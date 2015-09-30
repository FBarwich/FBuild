/*
* Any copyright is dedicated to the Public Domain.
* http://creativecommons.org/publicdomain/zero/1.0/*
*
* Author: Frank Barwich
*/

#include "ToolChain.h"

#include <boost/filesystem.hpp>


namespace ToolChain {

   static std::string toolchain;
   static std::string platform = "x86";

   static void CurrentFromEnvironment()
   {
      const char* envVersion = std::getenv("VisualStudioVersion");
      if (!envVersion) return;

      const char* envPath = std::getenv("PATH");
      if (!envPath) return;

      std::string version = envVersion;
      toolchain = "MSVC";

      for (char ch : version) if (ch != '.') toolchain += ch;

      std::string path = envPath;
   }

   static void LatestInstalledVersion()
   {
      std::string envString;
      for (size_t i = 20; i >= 8; --i) {
         envString = "VS" + std::to_string(i) + "0COMNTOOLS";

         const char* env = std::getenv(envString.c_str());
         if (env) {
            toolchain = "MSVC" + std::to_string(i) + "0";
            break;
         }
      }
   }
   

   void ToolChain(boost::string_ref newToolchain)
   {
      if (newToolchain.substr(0, 4) == "MSVC") {
         std::string envname = newToolchain.to_string();
         envname.erase(0, 4);
         envname.insert(0, "VS");
         envname += "COMNTOOLS";

         const char* commtoolsPathEnv = std::getenv(envname.c_str());
         if (!commtoolsPathEnv) throw std::runtime_error("Invalid ToolChain '" + newToolchain.to_string() + "' " + envname + " not found");

         ToolChain::toolchain.assign(newToolchain.begin(), newToolchain.end());
      }
      else if (newToolchain == "EMSCRIPTEN") {
         const char* emscriptenEnv = std::getenv("EMSCRIPTEN");
         if (!emscriptenEnv) throw std::runtime_error("Could not find environment variable 'EMSCRIPTEN'");

         ToolChain::toolchain.assign(newToolchain.begin(), newToolchain.end());
      }
      else {
         throw std::runtime_error("Unknown ToolChain " + newToolchain.to_string());
      }
   }

   std::string ToolChain()
   {
      if (toolchain.size()) return toolchain;
      CurrentFromEnvironment();
      if (toolchain.size()) return toolchain;
      LatestInstalledVersion();
      if (toolchain.size()) return toolchain;

      throw std::runtime_error("Unable to deduce Toolchain");
   }

   void Platform(boost::string_ref newPlatform)
   {
      if (newPlatform != "x86" && newPlatform != "x64") throw std::runtime_error("Only x86 and x64 platforms are supported");
      ToolChain::platform.assign(newPlatform.begin(), newPlatform.end());
   }


   std::string Platform()
   {
      return platform;
   }

   std::string SetEnvBatchCall()
   {
      auto toolschain = ToolChain();

      if (toolchain.substr(0, 4) == "MSVC") {
         auto envname = ToolChain();
         envname.erase(0, 4);
         envname.insert(0, "VS");
         envname += "COMNTOOLS";

         const auto commtoolsPathEnv = std::getenv(envname.c_str());
         if (!commtoolsPathEnv) throw std::runtime_error("Environmentvariable " + envname + " not found");

         auto batch = std::string{commtoolsPathEnv};
         batch += "../../VC/vcvarsall.bat";

         batch = boost::filesystem::canonical(batch).string();
         if (!boost::filesystem::exists(batch)) throw std::runtime_error(batch + " does not exist");

         auto cmd = "\"" + batch + "\" ";

         auto bin = std::string{commtoolsPathEnv};
         bin += "../../VC/bin";

         if (platform == "x64") {
            if (boost::filesystem::exists(bin + "/amd64")) cmd += "amd64";
            else cmd += "x86_amd64";
         }
         else {
            if (boost::filesystem::exists(bin + "/amd64_x86")) cmd += "amd64_x86";
            else cmd += "x86";
         }

         return "CALL " + cmd;
      }
      else if (toolchain == "EMSCRIPTEN") {
         const char* emscriptenEnv = std::getenv("EMSCRIPTEN");
         if (!emscriptenEnv) throw std::runtime_error("Could not find environment variable 'EMSCRIPTEN'");

         std::string batchfile = emscriptenEnv;
         batchfile += "/emsdk.bat";

         batchfile = boost::filesystem::canonical(batchfile).string();
         if (!boost::filesystem::exists(batchfile)) throw std::runtime_error(batchfile + " does not exist");

         return "CALL \"" + batchfile + "\" ";
      }
      else {
         throw std::runtime_error("Unknown ToolChain " + toolschain);
      }

   }
}
