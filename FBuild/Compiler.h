/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>
#include <memory>

#include <boost/algorithm/string.hpp>



class Compiler;


class ActualCompiler {
protected:
   Compiler& compiler;

   std::vector<std::string> outOfDate;

   std::vector<std::string> ObjFiles (const std::string& extension);
   std::vector<std::string> CompiledObjFiles (const std::string& extension);

public:
   ActualCompiler (Compiler& compiler) : compiler{compiler} { }
   virtual ~ActualCompiler () { }

   virtual void Compile () { }

   virtual std::vector<std::string> ObjFiles ()          { return std::vector<std::string>{}; }
   virtual std::vector<std::string> CompiledObjFiles ()  { return std::vector<std::string>{}; }
};







class ActualCompilerVisualStudio : public ActualCompiler {
   void CheckParams ();
   bool NeedsRebuild ();
   void DeleteOutOfDateObjectFiles ();
   void CompilePrecompiledHeaders ();
   void CompileFiles ();
   std::string CommandLine ();

public:
   ActualCompilerVisualStudio (Compiler& compiler) : ActualCompiler{compiler} { }

   void Compile () override;

   std::vector<std::string> ObjFiles() override         { return ActualCompiler::ObjFiles("obj"); }
   std::vector<std::string> CompiledObjFiles() override { return ActualCompiler::CompiledObjFiles("obj"); }
};







class Compiler {
   std::unique_ptr<ActualCompiler> actualCompiler;

   bool                     debug;
   std::string              objDir;
   std::vector<std::string> includes;
   std::vector<std::string> defines;
   std::vector<std::string> allFiles;
   bool                     crtStatic;
   int                      threads;
   std::string              args;
   std::string              precompiledHeader;
   std::string              precompiledCpp;
   bool                     dependencyCheck;
   int                      warnLevel;
   bool                     warningAsError;
   std::vector<int>         warningDisable;
   std::function<void()>    beforeCompile;

public:
   Compiler () : actualCompiler{new ActualCompiler{*this}}, threads{0}, debug{false}, crtStatic{false}, dependencyCheck{true}, warnLevel{1}, warningAsError{false} { }
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
   void BeforeCompile (std::function<void()> v)            { beforeCompile = std::move(v); }

   std::string                     Build () const             { return debug ? "Debug" : "Release"; }
   std::string                     CRT () const               { return crtStatic ? "Static" : "Dynamic"; }
   const std::string&              ObjDir () const            { return objDir; }
   const std::vector<std::string>& Includes () const          { return includes; }
   const std::vector<std::string>& Defines () const           { return defines; }
   const std::vector<std::string>& Files () const             { return allFiles; }
   int                             Threads () const           { return threads; }
   const std::string&              Args () const              { return args; }
   std::string                     PrecompiledHeader () const { return precompiledHeader.empty() ? "" : precompiledHeader + "; " + precompiledCpp; }
   std::string                     PrecompiledCPP () const    { return precompiledCpp; }
   std::string                     PrecompiledH () const      { return precompiledHeader; }
   bool                            DependencyCheck () const   { return dependencyCheck; }
   int                             WarnLevel () const         { return warnLevel; }
   bool                            WarningAsError () const    { return warningAsError; }
   const std::vector<int>&         WarningDisable () const    { return warningDisable; }
   const std::function<void()>&    BeforeCompile () const     { return beforeCompile; }

   void DoBeforeCompile () { if (beforeCompile) beforeCompile(); }

   void Compile ();

   std::vector<std::string> ObjFiles ()         { return actualCompiler->ObjFiles(); }
   std::vector<std::string> CompiledObjFiles () { return actualCompiler->CompiledObjFiles(); }
};

