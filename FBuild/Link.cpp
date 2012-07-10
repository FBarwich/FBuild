/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Link.h"

#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


void Link::Config (const std::string& v)
{
   if (v.empty()) debug = false;
   else {
      if (v != "Debug" && v != "Release") throw std::runtime_error("'Config' must be 'Release' or 'Debug' (Or empty for default, which is Release)");
      debug = v == "Debug";
   }
}


inline bool Exe (const std::string& filename)
{
   boost::filesystem::path file(filename);
   std::string ext = file.extension().string();
   boost::to_upper(ext);
   if (ext == ".EXE") return true;
   else if (ext == ".DLL") return false;
   else throw std::runtime_error("Invalid fileextension for FBuild.Link()");
}

void Link::Go ()
{
   if (files.empty()) return;
   if (output.empty()) throw std::runtime_error("Mising 'Output'");

   boost::filesystem::create_directories(boost::filesystem::path(output).remove_filename());

   std::string command = "Link -NOLOGO -INCREMENTAL:NO ";
   if (debug) command += "-DEBUG ";
   if (!Exe(output)) command += "-DLL ";
   command += "-OUT:\"" + output + "\" ";
   if (!importLib.empty()) command += "-IMPLIB:\"" + importLib + "\" ";

   std::for_each(libpath.cbegin(), libpath.cend(), [&command] (const std::string& f) { command += "-LIBPATH:\"" + f + "\" "; });
   std::for_each(libs.cbegin(), libs.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });
   std::for_each(files.cbegin(), files.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Link-Error");
}


void Link::AutoFilesFromCpp (const std::string& outdir, const std::vector<std::string>& cppFiles)
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

