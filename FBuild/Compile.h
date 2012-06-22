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
   bool                     debug;
   std::string              outDir;
   std::vector<std::string> includes;
   std::vector<std::string> files;
   bool                     crtStatic;
   int                      threads;
   std::string              cc;

public:
   Compile () : threads(0), debug(false), crtStatic(false) { }

   void Debug (bool debug)                      { this->debug = debug; }
   void OutDir (const std::string& outDir)      { this->outDir = outDir; }
   void AddInclude (const std::string& include) { includes.push_back(include); }
   void AddFile (const std::string& file)       { files.push_back(file); }
   void Threads (int threads)                   { this->threads = threads; }
   void CrtStatic (bool crtStatic)              { this->crtStatic = crtStatic; }
   void CC (const std::string& cc)              { this->cc += cc + " "; }

   void Go ();
};

