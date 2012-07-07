/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>


class Link {
   bool                     debug;
   std::string              output;
   std::string              importLib;
   std::vector<std::string> libpath;
   std::vector<std::string> libs;
   std::vector<std::string> files;

public:
   Link () : debug(false) { }

   void Config (const std::string& v);
   void Output (const std::string& v)          { output = v; }
   void ImportLib (const std::string& v)       { importLib = v; }
   void Libpath (std::vector<std::string>&& v) { libpath = std::move(v); }
   void Libs (std::vector<std::string>&& v)    { libs = std::move(v); }
   void Files (std::vector<std::string>&& v)   { files = std::move(v); }

   void Go ();
};

