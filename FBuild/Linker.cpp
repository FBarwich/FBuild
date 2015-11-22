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

#include <boost/filesystem.hpp>



inline bool Exe (const std::string& filename)
{
   boost::filesystem::path file(filename);
   std::string ext = file.extension().string();
   boost::to_upper(ext);
   if (ext == ".EXE") return true;
   else return false;
}




bool ActualLinker::NeedsRebuild () const
{
   if (!linker.DependencyCheck()) return true;
   if (!boost::filesystem::exists(linker.Output())) return true;

   std::time_t parentTime = boost::filesystem::last_write_time(linker.Output());

   for (auto&& file : linker.Files()) {
      if (boost::filesystem::last_write_time(file) > parentTime) return true;
   }

   for (auto&& lib : linker.Libs()) {
      for (auto&& path : linker.Libpath()) {
         const auto file = path + "/" + lib;
         if (boost::filesystem::exists(file)) {
            if (boost::filesystem::last_write_time(file) > parentTime) return true;
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

   if (boost::filesystem::exists(linker.Output())) boost::filesystem::remove(linker.Output());
   if (linker.ImportLib().size() && boost::filesystem::exists(linker.ImportLib())) boost::filesystem::remove(linker.ImportLib());

   boost::filesystem::create_directories(boost::filesystem::path(linker.Output()).remove_filename());

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
      auto rsp = boost::filesystem::temp_directory_path() / boost::filesystem::path(linker.Output()).filename();
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
         if (boost::filesystem::exists(path + "/" + lib)) {
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

   if (boost::filesystem::exists(linker.Output())) boost::filesystem::remove(linker.Output());

   boost::filesystem::create_directories(boost::filesystem::path(linker.Output()).remove_filename());

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

