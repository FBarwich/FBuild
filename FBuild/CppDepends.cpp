/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "CppDepends.h"
#include "Parser.h"
#include "BinaryStream.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/thread.hpp>



static boost::mutex lastWriteTimeMutex;
static std::unordered_map<std::string, std::time_t> lastWriteTimeCache;

std::time_t LastWriteTime (const std::string& file)
{
   boost::lock_guard<boost::mutex> lock(lastWriteTimeMutex);

   auto it = lastWriteTimeCache.find(file);
   if (it != lastWriteTimeCache.end()) return it->second;

   std::time_t ts = 0;
   try {
      ts = boost::filesystem::last_write_time(file);
   }
   catch (...) { }
   
   lastWriteTimeCache[file] = ts;

   return ts;
}






static const std::string includeString = "include";

static std::vector<boost::filesystem::path>& IncludePaths ()
{
   static std::vector<boost::filesystem::path> includes;
   return includes;
}





CppDepends::CppDepends (const boost::filesystem::path& file, bool ignoreCache)
{
   maxTime = 0;

   boost::filesystem::path f = boost::filesystem::canonical(file);
   f.make_preferred();

   if (!ignoreCache && CheckCache(f)) return;

   dependencies.clear();

   DoFile(f);

   WriteCache(f);
}

void CppDepends::DoFile (boost::filesystem::path file)
{
   file.make_preferred();

   if (dependencies.find(file.string()) != dependencies.end()) return;
   dependencies.insert(file.string());

   auto todo = Includes(file);

   auto parentPath = file.parent_path();

   std::for_each(todo.cbegin(), todo.cend(), [&] (const std::pair<char, std::string>& v) {
      if (v.first == '<') IncludeAnglebracketed(parentPath, v.second);
      else IncludeQuoted(parentPath, v.second);
   });
}

void CppDepends::IncludeQuoted (const boost::filesystem::path& path, const boost::filesystem::path& file)
{
   boost::filesystem::path include = path / file;

   if (boost::filesystem::exists(include) && boost::filesystem::is_regular_file(include)) DoFile(include);
   else IncludeAnglebracketed(path, file);
}

void CppDepends::IncludeAnglebracketed (const boost::filesystem::path& path, const boost::filesystem::path& file)
{
   const auto& includes = IncludePaths();
   for (size_t i = 0; i < includes.size(); ++i) {
      boost::filesystem::path include = includes[i] / file;
      if (boost::filesystem::exists(include) && boost::filesystem::is_regular_file(include)) {
         DoFile(include);
         return;
      }
   }

   boost::filesystem::path include = path / file;

   if (boost::filesystem::exists(include) && boost::filesystem::is_regular_file(include)) DoFile(include);
}


static boost::mutex includesMutex;
static std::unordered_map<std::string, std::vector<std::pair<char, std::string>>> includesCache;

std::vector<std::pair<char, std::string>> CppDepends::Includes (const boost::filesystem::path& file)
{
   {
      boost::lock_guard<boost::mutex> lock(includesMutex);
      auto it = includesCache.find(file.string());
      if (it != includesCache.end()) return it->second;
   }

   std::vector<std::pair<char, std::string>> includes;

   size_t size = static_cast<size_t>(boost::filesystem::file_size(file));

   boost::interprocess::file_mapping mapping(file.string().c_str(), boost::interprocess::read_only);
   boost::interprocess::mapped_region region(mapping, boost::interprocess::read_only, 0, size);

   const char* it = static_cast<const char*>(region.get_address());
   const char* end = it + size;

   it = std::find(it, end, '#');
   while (it != end) {
      ++it;
      it = SkipWhitespaces(it, end);
      auto itTmp = ConsumeIfEqual(it, end, includeString.cbegin(), includeString.cend());
      if (it != itTmp) {
         it = itTmp;
         it = SkipWhitespaces(it, end);

         auto itStart = it + 1;
         if (*it == '\"') {
            it = ConsumeUntil(itStart, end, '\"');
            if (it != itStart) includes.push_back(std::pair<char, std::string>('\"', std::string(itStart, it)));
         }
         else if (*it == '<') {
            it = ConsumeUntil(itStart, end, '>');
            if (it != itStart) includes.push_back(std::pair<char, std::string>('<', std::string(itStart, it)));
         }
      }

      it = std::find(it, end, '#');
   }

   boost::lock_guard<boost::mutex> lock(includesMutex);
   includesCache[file.string()] = includes;

   return std::move(includes);
}

bool CppDepends::CheckCache (const boost::filesystem::path& file)
{
   std::ifstream stream(file.string() + ":CppDepends_Cache", std::ofstream::in | std::ofstream::ate | std::ofstream::binary);
   if (!stream.good()) return false;
   if (stream.tellg() < sizeof(size_t)) return false;
   stream.seekg(0);

   std::string tmp;
   std::time_t ts;

   size_t count = 0;
   stream > count;
   for (size_t i = 0; i < count; ++i) {
      stream > tmp > ts;
      if (LastWriteTime(tmp) != ts) return false;
      dependencies.insert(tmp);
      if (ts > maxTime) maxTime = ts;
   }

   return true;
}

void CppDepends::WriteCache (const boost::filesystem::path& file)
{
   std::string writeMe;

   {
      std::stringstream ss;
      ss < dependencies.size();
      std::for_each(dependencies.begin(), dependencies.end(), [&] (const std::string& p) {
         std::time_t ts = LastWriteTime(p);
         ss < p < ts;
         if (ts > maxTime) maxTime = ts;
      });

      writeMe = std::move(ss.str());
   }

   std::time_t ts = boost::filesystem::last_write_time(file);

   {
      std::ofstream stream(file.string() + ":CppDepends_Cache", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
      if (!stream.good()) {
         std::cerr << "Error on writing cache for " << file << std::endl;
         return;
      }

      stream.write(writeMe.c_str(), writeMe.size());
   }

   boost::filesystem::last_write_time(file, ts);
}



void CppDepends::ClearIncludePath ()
{
   IncludePaths().clear();
}

void CppDepends::AddIncludePath (const boost::filesystem::path& path)
{
   auto p = boost::filesystem::canonical(path);
   p.make_preferred();

   if (!boost::filesystem::exists(p)) std::cout << "Include-Path " << p << " does not exist. Ignored.";
   else if (!boost::filesystem::is_directory(p)) std::cout << "Include-Path " << p << "is invalid. It's not a directory. Ignored";
   else IncludePaths().push_back(p);
}


















