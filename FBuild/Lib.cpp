/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Lib.h"

#include <cstdlib>
#include <algorithm>

#include <boost/filesystem.hpp>



bool Lib::NeedsRebuild () const
{
   if (!dependencyCheck) return true;
   if (!boost::filesystem::exists(output)) return true;

   std::time_t parentTime = boost::filesystem::last_write_time(output);

   for (size_t i = 0; i < files.size(); ++i) {
      if (boost::filesystem::last_write_time(files[i]) > parentTime) return true;
   }

   return false;
}

void Lib::Go () const
{
   if (files.empty()) return;
   if (output.empty()) throw std::runtime_error("Mising 'Output'");

   if (!NeedsRebuild()) return;

   boost::filesystem::create_directories(boost::filesystem::path(output).remove_filename());

   std::string command = "Lib -NOLOGO ";
   command += "-OUT:\"" + output + "\" ";
   
   std::for_each(files.cbegin(), files.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Error creating lib");
}


