/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "RC.h"

#include <algorithm>
#include <cstdlib>

#include <boost/filesystem.hpp>


std::string RC::Outfile (const std::string& infile) const
{
   boost::filesystem::path rc(infile);
   rc.replace_extension(".res");
   boost::filesystem::path out(outdir);
   auto outfile = out / rc.filename();
   return outfile.string();
}

bool RC::NeedsRebuild (const std::string& infile, const std::string& outfile) const
{
   if (!dependencyCheck) return true;

   if (!boost::filesystem::exists(outfile)) return true;

   return boost::filesystem::last_write_time(infile) > boost::filesystem::last_write_time(outfile); 
}

void RC::Go () const
{
   if (files.empty()) return;
   if (outdir.empty()) throw std::runtime_error("Mising 'Outdir'");

   std::for_each(files.cbegin(), files.cend(), [&] (const std::string& file) {
      std::string outfile = Outfile(file);
      if (NeedsRebuild(file, outfile)) {
         std::string command = "RC -nologo -fo\"" + outfile + "\" " + file;

         int r = std::system(command.c_str());
         if (r != 0) throw std::runtime_error("Error compiling resources");
      }
   });
}