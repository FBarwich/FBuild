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
   if (!Exe(output)) command += "-DLL ";
   command += "-OUT:\"" + output + "\" ";
   if (!importLib.empty()) command += "-IMPLIB:\"" + importLib + "\" ";

   std::for_each(libpath.cbegin(), libpath.cend(), [&command] (const std::string& f) { command += "-LIBPATH:\"" + f + "\" "; });
   std::for_each(libs.cbegin(), libs.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });
   std::for_each(files.cbegin(), files.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Link-Error");
}

