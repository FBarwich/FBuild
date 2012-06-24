/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Lib.h"

#include <cstdlib>
#include <algorithm>


void Lib::Go ()
{
   if (files.empty()) return;

   std::string command = "Lib -NOLOGO ";
   command += "-OUT:\"" + output + "\" ";
   
   std::for_each(files.cbegin(), files.cend(), [&command] (const std::string& f) { command += "\"" + f + "\" "; });

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Error creating lib");

}

