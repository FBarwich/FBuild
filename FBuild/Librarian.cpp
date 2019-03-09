/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Librarian.h"
#include "ToolChain.h"

#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>



bool ActualLibrarian::NeedsRebuild () const
{
   if (!librarian.DependencyCheck()) return true;
   if (!std::filesystem::exists(librarian.Output())) return true;

   const auto parentTime = std::filesystem::last_write_time(librarian.Output());

   for (auto&& f : librarian.Files()) {
      if (std::filesystem::last_write_time(f) > parentTime) return true;
   }

   return false;
}




void ActualLibrarianVisualStudio::Create ()
{
   if (librarian.Files().empty()) return;
   if (librarian.Output().empty()) throw std::runtime_error("Mising 'Output'");

   if (!NeedsRebuild()) return;

   std::cout << "\nCreating Lib (" << ToolChain::ToolChain() << " " << ToolChain::Platform() << ")" << std::endl;

   librarian.DoBeforeLink();

   if (std::filesystem::exists(librarian.Output())) std::filesystem::remove(librarian.Output());

   std::filesystem::create_directories(std::filesystem::path(librarian.Output()).remove_filename());

   std::string command = "-NOLOGO ";
   command += "-OUT:\"" + librarian.Output() + "\" ";
   
   for (auto&& f : librarian.Files()) command += "\"" + f + "\" ";

   if (command.size() > 8000) {
      auto rsp = std::filesystem::temp_directory_path() / std::filesystem::path(librarian.Output()).filename();
      rsp.replace_extension(".rsp");
      std::ofstream responseFile(rsp.string(), std::fstream::trunc);
      responseFile << command;
      command = "Lib @" + rsp.string();
   }
   else {
      command.insert(0, "Lib ");
   }

   std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
   int rc = std::system(cmd.c_str());
   if (rc != 0) throw std::runtime_error("Error creating lib");
}





void ActualLibrarianEmscripten::Create ()
{
   if (librarian.Files().empty()) return;
   if (librarian.Output().empty()) throw std::runtime_error("Mising 'Output'");

   if (!NeedsRebuild()) return;

   std::cout << "\nCreating Lib (" << ToolChain::ToolChain() << ")" << std::endl;

   librarian.DoBeforeLink();

   if (std::filesystem::exists(librarian.Output())) std::filesystem::remove(librarian.Output());

   std::filesystem::create_directories(std::filesystem::path(librarian.Output()).remove_filename());

   std::string command = "emcc -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 --memory-init-file 0 ";

   command += "-o \"" + librarian.Output() + "\" ";

   for (auto&& f : librarian.Files()) command += "\"" + f + "\" ";

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Error creating lib");
}





void Librarian::Create ()
{
   const auto toolChain = ToolChain::ToolChain();
   if (toolChain.substr(0, 4) == "MSVC") actualLibrarian.reset(new ActualLibrarianVisualStudio{*this});
   else if (toolChain == "EMSCRIPTEN") actualLibrarian.reset(new ActualLibrarianEmscripten{*this});
   else throw std::runtime_error("Unbekannte Toolchain: " + toolChain);

   actualLibrarian->Create();
}


