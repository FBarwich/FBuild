/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Linker.h"
#include "ToolChain.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>

inline bool Exe (const std::string& filename)
{
   std::filesystem::path file(filename);
   std::string ext = file.extension().string();

   for (char& ch : ext) ch = static_cast<char>(tolower(ch));

   return ext == ".exe";
}




bool ActualLinker::NeedsRebuild () const
{
   if (!linker.DependencyCheck()) return true;
   if (!std::filesystem::exists(linker.Output())) return true;

   const auto parentTime = std::filesystem::last_write_time(linker.Output());

   for (auto&& file : linker.Files()) {
      if (std::filesystem::last_write_time(file) > parentTime) return true;
   }

   for (auto&& lib : linker.Libs()) {
      for (auto&& path : linker.Libpath()) {
         const auto file = path + "/" + lib;
         if (std::filesystem::exists(file)) {
            if (std::filesystem::last_write_time(file) > parentTime) return true;
         }
      }
   }

   return false;
}






void ActualLinkerVisualStudio::Link ()
{
   if (linker.Files().empty()) return;
   if (linker.Output().empty()) throw std::runtime_error("Mising 'Output'");

   if (!NeedsRebuild()) return;

   std::cout << "\nLinking (" << ToolChain::ToolChain() << " " << ToolChain::Platform() << ")" << std::endl;

   linker.DoBeforeLink();

   std::filesystem::create_directories(std::filesystem::path(linker.Output()).remove_filename());

   bool debug = linker.Build() == "Debug";

   std::string command = "-NOLOGO -INCREMENTAL:NO -LARGEADDRESSAWARE -STACK:3000000 ";
   if (debug) command += "-DEBUG ";
   if (!Exe(linker.Output())) command += "-DLL ";
   command += "-OUT:\"" + linker.Output() + "\" ";
   if (!linker.ImportLib().empty()) command += "-IMPLIB:\"" + linker.ImportLib() + "\" ";
   if (!linker.Def().empty()) command += "-DEF:\"" + linker.Def() + "\" ";
   if (!linker.Args().empty()) command += linker.Args() + " ";

   for (auto&& f : linker.Libpath()) command += "-LIBPATH:\"" + f + "\" ";
   for (auto&& f : linker.Libs()) command += "\"" + f + "\" ";
   for (auto&& f : linker.Files()) command += "\"" + f + "\" ";

   const char* env = std::getenv("FB_LINKER");
   if (env) command += std::string(env) + " ";

   if (debug) {
      env = std::getenv("FB_LINKER_DEBUG");
      if (env) command += std::string(env) + " ";
   }
   else {
      env = std::getenv("FB_LINKER_RELEASE");
      if (env) command += std::string(env) + " ";
   }

   if (command.size() > 8000) {
      auto rsp = std::filesystem::temp_directory_path() / std::filesystem::path(linker.Output()).filename();
      rsp.replace_extension(".rsp");
      std::ofstream responseFile(rsp.string(), std::fstream::trunc);
      responseFile << command;
      command = "link @" + rsp.string();
   }
   else {
      command.insert(0, "link ");
   }

   std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
   int rc = std::system(cmd.c_str());
   if (rc != 0) throw std::runtime_error("Link-Error");
}







std::vector<std::string> ActualLinkerEmscripten::LibsWithPath () const
{
   std::vector<std::string> result;

   const auto& libs = linker.Libs();

   for (auto&& lib : libs) {
      bool gotIt = false;

      for (auto&& path : linker.Libpath()) {
         if (std::filesystem::exists(path + "/" + lib)) {
            result.push_back(path + "/" + lib);
            gotIt = true;
         }
      }

      if (!gotIt) result.push_back(lib);
   }

   return result;
}

void ActualLinkerEmscripten::Link ()
{
   if (linker.Files().empty()) return;
   if (linker.Output().empty()) throw std::runtime_error("Mising 'Output'");

   if (!NeedsRebuild()) return;

   std::cout << "\nLinking (" << ToolChain::ToolChain() << ")" << std::endl;

   linker.DoBeforeLink();

   if (std::filesystem::exists(linker.Output())) std::filesystem::remove(linker.Output());

   std::filesystem::create_directories(std::filesystem::path(linker.Output()).remove_filename());

   bool debug = linker.Build() == "Debug";


   std::string command = "emcc -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 --memory-init-file 0 ";

   if (debug) command += "-g -O1 -D_DEBUG ";
   else command += "-O3 -DNDEBUG ";

   if (!linker.Args().empty()) command += linker.Args() + " ";

   command += "-o \"" + linker.Output() + "\" ";

   for (auto&& f : linker.Files()) command += "\"" + f + "\" ";
   for (auto&& f : LibsWithPath()) command += "\"" + f + "\" ";

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Link-Error");
}








void Linker::Link ()
{
   const auto toolChain = ToolChain::ToolChain();
   if (toolChain.substr(0, 4) == "MSVC") actualLinker.reset(new ActualLinkerVisualStudio{*this});
   else if (toolChain == "EMSCRIPTEN") actualLinker.reset(new ActualLinkerEmscripten{*this});
   else throw std::runtime_error("Unbekannte Toolchain: " + toolChain);

   actualLinker->Link();
}

