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
#include <iostream>

#include <boost/filesystem.hpp>



bool ActualLibrarian::NeedsRebuild () const
{
   if (!librarian.DependencyCheck()) return true;
   if (!boost::filesystem::exists(librarian.Output())) return true;

   std::time_t parentTime = boost::filesystem::last_write_time(librarian.Output());

   for (auto&& f : librarian.Files()) {
      if (boost::filesystem::last_write_time(f) > parentTime) return true;
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

   if (boost::filesystem::exists(librarian.Output())) boost::filesystem::remove(librarian.Output());

   boost::filesystem::create_directories(boost::filesystem::path(librarian.Output()).remove_filename());

   std::string command = "Lib -NOLOGO ";
   command += "-OUT:\"" + librarian.Output() + "\" ";
   
   for (auto&& f : librarian.Files()) command += "\"" + f + "\" ";

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

   if (boost::filesystem::exists(librarian.Output())) boost::filesystem::remove(librarian.Output());

   boost::filesystem::create_directories(boost::filesystem::path(librarian.Output()).remove_filename());

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


