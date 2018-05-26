/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <unordered_set>
#include <ctime>
#include <iostream>
#include <filesystem>



class CppDepends {
public:
   CppDepends (const std::filesystem::path& file, bool ignoreCache = false);

   typedef std::unordered_set<std::string>::const_iterator Iterator;

   Iterator Begin () const { return dependencies.cbegin(); }
   Iterator End () const { return dependencies.cend(); }

   size_t Size () const { return dependencies.size(); }

   uint64_t MaxTime () const { return maxTime; }

   // Beware. These functions are NOT threadsafe
   static void ClearIncludePath ();
   static void AddIncludePath (const std::filesystem::path& path);
   static void PrecompiledHeader (const std::string& prec);

private:
   std::unordered_set<std::string> dependencies{};
   uint64_t maxTime{0};

   void DoFile (std::filesystem::path file);
   std::vector<std::pair<char, std::string>> Includes (const std::filesystem::path& file);

   void IncludeQuoted (const std::filesystem::path& path, const std::filesystem::path& file);
   void IncludeAnglebracketed (const std::filesystem::path& path, const std::filesystem::path& file);

   bool CheckCache (const std::filesystem::path& file);
   void WriteCache (const std::filesystem::path& file);
};


