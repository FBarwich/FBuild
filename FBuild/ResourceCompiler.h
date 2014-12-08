/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>

class ResourceCompiler {
   std::string outdir;
   std::vector<std::string> files;
   std::vector<std::string> includes;

   bool dependencyCheck;

   std::string Outfile (const std::string& infile) const;
   bool NeedsRebuild (const std::string& infile, const std::string& outfile) const;

public:
   ResourceCompiler () : dependencyCheck(true) { }

   void Outdir(std::string v)                { outdir = std::move(v); }
   void Files(std::vector<std::string> v)    { files = std::move(v); }
   void Includes(std::vector<std::string> v) { includes = std::move(v); }
   void DependencyCheck(bool v)              { dependencyCheck = v; }

   const std::string&              Outdir () const          { return outdir; }
   const std::vector<std::string>& Files () const           { return files; }
   const std::vector<std::string>& Includes() const         { return includes; }
   bool                            DependencyCheck() const { return dependencyCheck; }

   void Compile () const;

   std::vector<std::string> Outfiles () const;
};

