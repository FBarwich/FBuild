/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Link.h"

#include <algorithm>
#include <fstream>

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

   if (!NeedsRebuild()) return;

   boost::filesystem::create_directories(boost::filesystem::path(output).remove_filename());

   std::string command = "-NOLOGO -INCREMENTAL:NO ";
   if (debug) command += "-DEBUG ";
   if (!Exe(output)) command += "-DLL ";
   command += "-OUT:\"" + output + "\" ";
   if (!importLib.empty()) command += "-IMPLIB:\"" + importLib + "\" ";
   if (!def.empty()) command += "-DEF:\"" + def + "\" ";

   std::for_each(libpath.cbegin(), libpath.cend(), [&command] (const std::string& f) { command += "-LIBPATH:\"" + f + "\" "; });
   std::for_each(libs.cbegin(), libs.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });
   std::for_each(files.cbegin(), files.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });

   if (command.size() > 8000) {
      auto rsp = boost::filesystem::temp_directory_path() / boost::filesystem::path(output).filename();
      rsp.replace_extension(".rsp");
      std::ofstream responseFile(rsp.string(), std::fstream::trunc);
      responseFile << command;
      command = "link @" + rsp.string();
   }
   else {
      command.insert(0, "link ");
   }


   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Link-Error");
}

bool Link::NeedsRebuild () const
{
   if (!dependencyCheck) return true;
   if (!boost::filesystem::exists(output)) return true;

   std::time_t parentTime = boost::filesystem::last_write_time(output);

   for (size_t i = 0; i < files.size(); ++i) {
      if (boost::filesystem::last_write_time(files[i]) > parentTime) return true;
   }

   return false;
}

std::string Link::Output () const
{
   boost::filesystem::path p(output);
   p.make_preferred();
   return p.string();
}

