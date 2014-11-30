/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "ResourceCompiler.h"
#include "CppDepends.h"
#include "ToolChain.h"

#include <algorithm>
#include <cstdlib>

#include <boost/filesystem.hpp>


std::string ResourceCompiler::Outfile (const std::string& infile) const
{
   boost::filesystem::path rc(infile);
   rc.replace_extension(".res");
   boost::filesystem::path out(outdir);
   auto outfile = out / rc.filename();
   return outfile.string();
}

bool ResourceCompiler::NeedsRebuild (const std::string& infile, const std::string& outfile) const
{
   if (!dependencyCheck) return true;
   if (!boost::filesystem::exists(outfile)) return true;

   CppDepends dep(infile);
   return boost::filesystem::last_write_time(outfile) < dep.MaxTime(); 
}

void ResourceCompiler::Compile () const
{
   if (files.empty()) return;
   if (outdir.empty()) throw std::runtime_error("Mising 'Outdir'");

   if (!boost::filesystem::exists(outdir)) boost::filesystem::create_directories(outdir);

   std::for_each(files.cbegin(), files.cend(), [&] (const std::string& file) {
      std::string outfile = Outfile(file);
      if (NeedsRebuild(file, outfile)) {
         if (boost::filesystem::exists(outfile)) boost::filesystem::remove(outfile);
         std::string command = "RC -nologo -fo\"" + outfile + "\" " + file;

         std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
         int rc = std::system(cmd.c_str());
         if (rc != 0) throw std::runtime_error("Error compiling resources");
      }
   });
}

std::vector<std::string> ResourceCompiler::Outfiles () const
{
   std::vector<std::string> result;

   std::for_each(files.begin(), files.end(), [&] (const std::string& file) {
      result.push_back(Outfile(file));
   });

   return result;
}

