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
#include <memory>

#include <boost/algorithm/string.hpp>


class Linker;

class ActualLinker {
protected:
   Linker& linker;

   bool NeedsRebuild () const;

public:
   ActualLinker (Linker& linker) : linker{linker} { }
   virtual ~ActualLinker () { }

   virtual void Link () { }
};






class ActualLinkerVisualStudio : public ActualLinker {
public:
   ActualLinkerVisualStudio (Linker& linker) : ActualLinker{linker} { }

   void Link () override;
};






class ActualLinkerEmscripten : public ActualLinker {
   std::vector<std::string> LibsWithPath () const;

public:
   ActualLinkerEmscripten (Linker& linker) : ActualLinker{linker} { }

   void Link () override;
};







class Linker {
   std::unique_ptr<ActualLinker> actualLinker;

   bool                     debug;
   std::string              output;
   std::string              importLib;
   std::string              def;
   std::vector<std::string> libpath;
   std::vector<std::string> libs;
   std::vector<std::string> files;
   bool                     dependencyCheck;
   std::string              args;
   std::function<void()>    beforeLink;

public:
   Linker () : actualLinker{new ActualLinker{*this}}, debug{false}, dependencyCheck{true} { }

   void Build (std::string v)
   {
      boost::to_lower(v);
      if (v == "release") debug = false;
      else if (v == "debug") debug = true;
      else throw std::runtime_error("Excpected <Release> or <Debug> for Build");
   }
   void Output (std::string v)                       { output = std::move(v); }
   void ImportLib (std::string v)                    { importLib = std::move(v); }
   void Def (std::string v)                          { def = std::move(v); }
   void Libpath (std::vector<std::string> v)         { libpath = std::move(v); }
   void Libs (std::vector<std::string> v)            { libs = std::move(v); }
   void Files (std::vector<std::string> v)           { files = std::move(v); }
   void AddFiles (const std::vector<std::string>& v) { std::copy(v.begin(), v.end(), std::back_inserter(files));  }
   void DependencyCheck (bool v)                     { dependencyCheck = v; }
   void Args (std::string v)                         { args = std::move(v); }
   void BeforeLink (std::function<void()> v)         { beforeLink = std::move(v); }

   std::string                     Build () const           { return debug ? "Debug" : "Release"; }
   const std::string               Output () const          { return output; }
   const std::string               ImportLib () const       { return importLib; }
   const std::string               Def () const             { return def; }
   const std::vector<std::string>& Libpath () const         { return libpath; }
   const std::vector<std::string>& Libs () const            { return libs; }
   const std::vector<std::string>& Files () const           { return files; }
   bool                            DependencyCheck () const { return dependencyCheck; }
   const std::string&              Args () const            { return args; }
   const std::function<void()>&    BeforeLink () const      { return beforeLink; }

   void DoBeforeLink () { if (beforeLink) beforeLink(); }

   void Link ();
};

