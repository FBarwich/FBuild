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
   std::vector<std::string> defines;
   std::vector<std::string> files;
   bool                     crtStatic;
   int                      threads;
   std::string              cc;
   std::string              precompiledHeader;
   std::string              precompiledCpp;

   std::string CommandLine () const;

   void CompilePrecompiledHeaders ();
   void CompileFiles ();

public:
   Compile () : threads(0), debug(false), crtStatic(false) { }

   void Config (const std::string& v);
   void CRT (const std::string& v);
   void OutDir (const std::string& outDir)       { this->outDir = outDir; }
   void Threads (int threads)                    { this->threads = threads; }
   void CC (const std::string& cc)               { this->cc += cc + " "; }
   void PrecompiledHeader (const std::string& v) { precompiledHeader = v; }
   void PrecompiledCpp (const std::string& v)    { precompiledCpp = v; }
   void Include (std::vector<std::string>&& v)   { includes = std::move(v); }
   void Define (std::vector<std::string>&& v)    { defines = std::move(v); }
   void Files (std::vector<std::string>&& v)     { files = std::move(v); }

   void Go ();
};

