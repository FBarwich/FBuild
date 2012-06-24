/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>

class FileOutOfDate {
   std::string              parent;
   std::vector<std::string> files;

public:

   void Parent (const std::string& v) { parent = v; }
   void AddFile (const std::string& v) { files.push_back(v); }

   bool Go ();
};

