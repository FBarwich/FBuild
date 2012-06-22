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

#include <boost/filesystem.hpp>



class CppDepends {
public:
   CppDepends (const boost::filesystem::path& file, bool ignoreCache = false);

   typedef std::unordered_set<std::string>::const_iterator Iterator;

   Iterator Begin () const { return dependencies.cbegin(); }
   Iterator End () const { return dependencies.cend(); }

   size_t Size () const { return dependencies.size(); }

   std::time_t MaxTime () const { return maxTime; }

   // Beware. This functions are NOT threadsafe
   static void ClearIncludePath ();
   static void AddIncludePath (const boost::filesystem::path& path);

private:
   std::unordered_set<std::string> dependencies;
   std::time_t maxTime;

   void DoFile (boost::filesystem::path file);
   std::vector<std::pair<char, std::string>> Includes (const boost::filesystem::path& file);

   void IncludeQuoted (const boost::filesystem::path& path, const boost::filesystem::path& file);
   void IncludeAnglebracketed (const boost::filesystem::path& path, const boost::filesystem::path& file);

   bool CheckCache (const boost::filesystem::path& file);
   void WriteCache (const boost::filesystem::path& file);
};


