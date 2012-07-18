/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>
#include <iterator>

class FileOutOfDate {
   std::string              parent;
   std::vector<std::string> files;

public:

   void Parent (const std::string& v)                { parent = v; }
   void AddFile (const std::string& v)               { files.push_back(v); }
   void AddFiles (const std::vector<std::string>& f) { std::copy(f.cbegin(), f.cend(), back_inserter(files)); }

   bool Go () const;
};

