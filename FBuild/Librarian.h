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

class Librarian {
   std::string              output;
   std::vector<std::string> files;
   bool                     dependencyCheck;
   std::function<void()>    beforeLink;

   bool NeedsRebuild () const;

public:

   void Output (std::string v)                       { output = std::move(v); }
   void Files (std::vector<std::string> v)           { files = std::move(v); }
   void AddFile (const std::string& file)            { files.push_back(file); }
   void AddFiles (const std::vector<std::string>& f) { std::copy(f.cbegin(), f.cend(), back_inserter(files)); }
   void DependencyCheck (bool v)                     { dependencyCheck = v; }
   void BeforeLink (std::function<void()> v)         { beforeLink = std::move(v); }

   const std::string&              Output () const          { return output; }
   const std::vector<std::string>& Files () const           { return files; }
   bool                            DependencyCheck () const { return dependencyCheck; }
   const std::function<void()>&    BeforeLink () const      { return beforeLink; }

   void Create () const;
};

