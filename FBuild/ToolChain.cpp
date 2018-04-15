/*
* Any copyright is dedicated to the Public Domain.
* http://creativecommons.org/publicdomain/zero/1.0/*
*
* Author: Frank Barwich
*/

#include "ToolChain.h"

#include <boost/filesystem.hpp>

#include <iostream>


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
      if (newToolchain == "MSVC") {
         CurrentFromEnvironment();
         if (toolchain.empty()) LatestInstalledVersion();
         if (toolchain.empty()) throw std::runtime_error("Unable to deduce MSVC-Version");
      }
      else if (newToolchain.substr(0, 4) == "MSVC") {
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
      auto tchain = ToolChain();

      if (tchain.substr(0, 4) == "MSVC") {
         auto envname = ToolChain();
         envname.erase(0, 4);
         envname.insert(0, "VS");
         envname += "COMNTOOLS";

         auto commtoolsPathEnv = std::getenv(envname.c_str());
         if (!commtoolsPathEnv) {
            commtoolsPathEnv = std::getenv("VSAPPIDDIR"); // Aaaaargh. This points to the IDE-Directory. VS2017 no longer sets an COMTOOLSxxx Env. Except if you're building from the VS2017 commandline, then COMTOOLSxxx is set. So... Yeah...

            if (!commtoolsPathEnv) throw std::runtime_error("Environmentvariable " + envname + " not found");
         }

         auto batch = std::string{commtoolsPathEnv};
         batch += "../../VC/vcvarsall.bat";

         if (!boost::filesystem::exists(batch)) {
            auto batch2 = std::string{commtoolsPathEnv};
            batch2 += "../../VC/Auxiliary/Build/vcvarsall.bat";  // New path in VC2017
            if (!boost::filesystem::exists(batch2)) {
               throw std::runtime_error("Neither\n" + batch + "\nor\n" + batch2 + " does exist");
            }
            else {
               batch = batch2;
            }
         }

         batch = boost::filesystem::canonical(batch).string();

         auto cmd = "\"" + batch + "\" ";

         auto bin = std::string{commtoolsPathEnv};
         bin += "../../VC/bin";

         if (platform == "x64") {
            if (batch.find("Community") != std::string::npos || boost::filesystem::exists(bin + "/amd64")) cmd += "amd64"; // Fucking Hell :(
            else cmd += "x86_amd64";
         }
         else {
            if (batch.find("Community") != std::string::npos || boost::filesystem::exists(bin + "/amd64_x86")) cmd += "amd64_x86";
            else cmd += "x86";
         }

         return "CALL " + cmd + " >nul";   // vcvarsall.bat spews out redundant crap -> nul    
      }
      else if (tchain == "EMSCRIPTEN") {
         const char* emscriptenEnv = std::getenv("EMSCRIPTEN");
         if (!emscriptenEnv) throw std::runtime_error("Could not find environment variable 'EMSCRIPTEN'");

         std::string batchfile = emscriptenEnv;
         for (auto&& ch : batchfile) if (ch == '/') ch = '\\';  // 1.35 writes it's Environment-Variable with Slashes. Previous Versions used Backslashes (at least the 1.34.1 that's on their webpage did).
         auto pos = batchfile.rfind('\\');      // The environment holds the path to emscripten
         pos = batchfile.rfind('\\', pos - 1);  // plus the string "\emscripten\<version>", where version ist the version, eg 1.34.1
         batchfile.erase(pos);                  // We have to erase the information after the actual path.

         batchfile += "\\emsdk_env.bat";

         if (!boost::filesystem::exists(batchfile)) throw std::runtime_error(batchfile + " does not exist");

         return "CALL \"" + batchfile + "\" >nul ";
      }
      else {
         throw std::runtime_error("Unknown ToolChain " + tchain);
      }

   }
}
