/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Compile.h"

#include <iostream>
#include <algorithm>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>


void Compile::Go ()
{
   if (files.empty()) return;

   boost::filesystem::path out(outDir);
   if (!boost::filesystem::exists(out)) boost::filesystem::create_directory(out);

   std::string command = "cl.exe -nologo -c -EHa -DWIN32 ";
   if (threads) command += "-MP" + boost::lexical_cast<std::string>(threads) + " ";
   else command += "-MP ";
   command += "-Fo\"" + out.string() + "\" ";
   std::for_each(includes.cbegin(), includes.cend(), [&command] (const std::string& s) { command += "-I\"" + s + "\" "; });
   std::for_each(files.cbegin(), files.cend(), [&command] (const std::string& s) { command += "\"" + s + "\" "; });

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");
}