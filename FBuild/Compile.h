/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>

class Compile {
   std::string              outDir;
   std::vector<std::string> includes;
   std::vector<std::string> files;
   std::vector<std::string> objectFiles;
   int                      threads;

   void AddObjectFile (const std::string& objFile) { objectFiles.push_back(objFile); }

public:
   Compile () : threads(0) { }

   void OutDir (const std::string& outDir)      { this->outDir = outDir; }
   void AddInclude (const std::string& include) { includes.push_back(include); }
   void AddFile (const std::string& file)       { files.push_back(file); }
   void Threads (int threads)                   { this->threads = threads; }

   void Go ();

   const std::vector<std::string> ObjectFiles () const { return objectFiles; }
};

