/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>
#include <filesystem>

class Copy {
   std::string source;
   std::string dest;
   bool        ignoreTimestamp;
   int         copied;

   struct CopyMe {
      std::filesystem::path source;
      std::filesystem::path dest;
   };

   std::vector<CopyMe> copyMe;

   void DoFile (std::filesystem::path sourceFile, std::filesystem::path destFile);

   void CheckParams () const;

public:
   Copy () : ignoreTimestamp(false), copied(0) { }

   void Source (std::string v) { source = std::move(v); }
   void Dest (std::string v)   { dest = std::move(v); }
   void DependencyCheck (bool v) { ignoreTimestamp = !v; }

   const std::string& Source () const         { return source; }
   const std::string& Dest () const           { return dest; }
   bool               DependencyCheck() const { return !ignoreTimestamp; }

   bool NeedsCopy () const;

   int Go ();
};




