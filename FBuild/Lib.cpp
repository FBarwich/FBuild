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


void Lib::Go ()
{
   if (files.empty()) return;
   if (output.empty()) throw std::runtime_error("Mising 'Output'");

   boost::filesystem::create_directories(boost::filesystem::path(output).remove_filename());

   std::string command = "Lib -NOLOGO ";
   command += "-OUT:\"" + output + "\" ";
   
   std::for_each(files.cbegin(), files.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Error creating lib");
}

void Lib::AutoFilesFromCpp (const std::string& outdir, const std::vector<std::string>& cppFiles)
{
   boost::filesystem::path objdir(outdir);
   boost::filesystem::path file;

   std::for_each(cppFiles.begin(), cppFiles.end(), [&] (const std::string& s) {
      file = s;
      auto obj = objdir / file.filename();
      obj.replace_extension(".obj");
      files.push_back(obj.string());
   });

   bool doIt = false;

   if (boost::filesystem::exists(output)) {
      std::time_t outputTime = boost::filesystem::last_write_time(output);

      for (size_t i = 0; i < files.size(); ++i) {
         if (boost::filesystem::last_write_time(files[i]) > outputTime) {
            doIt = true;
            break;
         }
      }
   }
   else {
      doIt = true;
   }

   if (!doIt) files.clear();
}

