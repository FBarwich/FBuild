/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>

class RC {
   std::string outdir;
   std::vector<std::string> files;
   bool dependencyCheck;

   std::string Outfile (const std::string& infile) const;
   bool NeedsRebuild (const std::string& infile, const std::string& outfile) const;

public:
   RC () : dependencyCheck(true) { }

   void Outdir (const std::string& v)        { outdir = v; }
   void Files (std::vector<std::string>&& v) { files = std::move(v); }
   void DependencyCheck (bool v)             { dependencyCheck = v; }

   void Go () const;
};

