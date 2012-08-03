/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

class Compiler {
   bool                     debug;
   std::string              objDir;
   std::vector<std::string> includes;
   std::vector<std::string> defines;
   std::vector<std::string> allFiles;
   std::vector<std::string> outOfDate;
   bool                     crtStatic;
   int                      threads;
   std::string              args;
   std::string              precompiledHeader;
   std::string              precompiledCpp;
   bool                     dependencyCheck;
   int                      warnLevel;
   bool                     warningAsError;
   std::vector<int>         warningDisable;

   std::string CommandLine () const;

   void CompilePrecompiledHeaders ();
   void CompileFiles ();

   bool NeedsRebuild ();

   void CheckParams ();

public:
   Compiler () : threads(0), debug(false), crtStatic(false), dependencyCheck(true), warnLevel(1), warningAsError(false) { }
   ~ Compiler () { }

   void Build (std::string build) 
   {
      boost::to_lower(build);
      if (build == "release") debug = false;
      else if (build == "debug") debug = true;
      else throw std::runtime_error("Excpected <Release> or <Debug> for Build");
   }
   void CRT (const std::string& v)
   {
      if (v == "Static") crtStatic = true;
      else if (v == "Dynamic") crtStatic = false;
      else throw std::runtime_error("Excpected <Dynamic> or <Static> for CRT");
   }
   void ObjDir (std::string v)                             { objDir = std::move(v); }
   void Includes (std::vector<std::string> v)              { includes = std::move(v); }
   void Defines (std::vector<std::string> v)               { defines = std::move(v); }
   void Files (std::vector<std::string> v)                 { allFiles = std::move(v); }
   void Threads (int v)                                    { threads = v; }
   void Args (std::string v)                               { args = std::move(v); }
   void PrecompiledHeader (std::string h, std::string cpp) { precompiledHeader = std::move(h); precompiledCpp = std::move(cpp); }
   void DependencyCheck (bool v)                           { dependencyCheck = v; }
   void WarnLevel (int v)                                  { warnLevel = v; }
   void WarningAsError (bool v)                            { warningAsError = v; }
   void WarningDisable (std::vector<int> v)                { warningDisable = std::move(v); }

   std::string                     Build () const             { return debug ? "Debug" : "Release"; }
   std::string                     CRT () const               { return crtStatic ? "Static" : "Dynamic"; }
   const std::string&              ObjDir () const            { return objDir; }
   const std::vector<std::string>& Includes () const          { return includes; }
   const std::vector<std::string>& Defines () const           { return defines; }
   const std::vector<std::string>& Files () const             { return allFiles; }
   int                             Threads () const           { return threads; }
   const std::string&              Args () const              { return args; }
   std::string                     PrecompiledHeader () const { return precompiledHeader.empty() ? "" : precompiledHeader + "; " + precompiledCpp; }
   bool                            DependencyCheck () const   { return dependencyCheck; }
   int                             WarnLevel () const         { return warnLevel; }
   bool                            WarningAsError () const    { return warningAsError; }
   const std::vector<int>&         WarningDisable () const    { return warningDisable; }

   void Compile ();

   std::vector<std::string> ObjFiles () const;
   std::vector<std::string> CompiledObjFiles () const;
};

