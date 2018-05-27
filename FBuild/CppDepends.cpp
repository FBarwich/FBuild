/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "CppDepends.h"
#include "Parser.h"
#include "BinaryStream.h"
#include "MemoryMappedFile.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <mutex>

#include <boost/thread.hpp>



static std::mutex lastWriteTimeMutex;
static std::unordered_map<std::string, uint64_t> lastWriteTimeCache;
static std::string precompiledHeader;

uint64_t LastWriteTime (const std::string& file)
{
   std::lock_guard lock(lastWriteTimeMutex);

   auto it = lastWriteTimeCache.find(file);
   if (it != lastWriteTimeCache.end()) return it->second;

   uint64_t ts = 0;
   try {
      const std::filesystem::file_time_type timestamp = std::filesystem::last_write_time(file);
      ts = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
   }
   catch (...) { }
   
   lastWriteTimeCache[file] = ts;

   return ts;
}






static const std::string includeString = "include";

static std::vector<std::filesystem::path>& IncludePaths ()
{
   static std::vector<std::filesystem::path> includes;
   return includes;
}





CppDepends::CppDepends (const std::filesystem::path& file, bool ignoreCache)
{
   maxTime = 0;

   std::filesystem::path f = std::filesystem::canonical(file);
   f.make_preferred();

   if (!ignoreCache && CheckCache(f)) return;

   dependencies.clear();

   if (precompiledHeader.size()) DoFile(std::filesystem::canonical(precompiledHeader));
   DoFile(f);

   WriteCache(f);
}

void CppDepends::DoFile (std::filesystem::path file)
{
   file.make_preferred();

   if (dependencies.find(file.string()) != dependencies.end()) return;
   dependencies.insert(file.string());

   const auto todo = Includes(file);

   const auto parentPath = file.parent_path();

   std::for_each(todo.cbegin(), todo.cend(), [&] (const std::pair<char, std::string>& v) {
      if (v.first == '<') IncludeAnglebracketed(parentPath, v.second);
      else IncludeQuoted(parentPath, v.second);
   });
}

void CppDepends::IncludeQuoted (const std::filesystem::path& path, const std::filesystem::path& file)
{
   std::filesystem::path include = path / file;

   if (std::filesystem::exists(include) && std::filesystem::is_regular_file(include)) DoFile(include);
   else IncludeAnglebracketed(path, file);
}

void CppDepends::IncludeAnglebracketed (const std::filesystem::path& path, const std::filesystem::path& file)
{
   const auto& includes = IncludePaths();
   for (size_t i = 0; i < includes.size(); ++i) {
      std::filesystem::path include = includes[i] / file;
      if (std::filesystem::exists(include) && std::filesystem::is_regular_file(include)) {
         DoFile(include);
         return;
      }
   }

   std::filesystem::path include = path / file;

   if (std::filesystem::exists(include) && std::filesystem::is_regular_file(include)) DoFile(include);
}


static std::mutex includesMutex;
static std::unordered_map<std::string, std::vector<std::pair<char, std::string>>> includesCache;

std::vector<std::pair<char, std::string>> CppDepends::Includes (const std::filesystem::path& file)
{
   {
      std::lock_guard lock(includesMutex);
      auto it = includesCache.find(file.string());
      if (it != includesCache.end()) return it->second;
   }

   std::vector<std::pair<char, std::string>> includes;

   const MemoryMappedFile mmf{file};
   const char* it = mmf.CBegin();
   const char* end = mmf.CEnd();

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
            if (it != itStart) includes.emplace_back(std::pair<char, std::string>('\"', std::string(itStart, it)));
         }
         else if (*it == '<') {
            it = ConsumeUntil(itStart, end, '>');
            if (it != itStart) includes.emplace_back(std::pair<char, std::string>('<', std::string(itStart, it)));
         }
      }

      it = std::find(it, end, '#');
   }

   std::lock_guard lock(includesMutex);
   includesCache[file.string()] = includes;

   return std::move(includes);
}

bool CppDepends::CheckCache (const std::filesystem::path& file)
{
   std::ifstream stream(file.string() + ":CppDepends_Cache5", std::ofstream::in | std::ofstream::ate | std::ofstream::binary);
   if (!stream.good()) return false;
   if (stream.tellg() < sizeof(size_t)) return false;
   stream.seekg(0);

   std::string tmp;
   uint64_t ts;

   stream > tmp;
   if (tmp != file.string()) return false;

   uint32_t count = 0;
   stream > count;
   for (uint32_t i = 0; i < count; ++i) {
      stream > tmp > ts;
      if (LastWriteTime(tmp) != ts) return false;
      dependencies.insert(tmp);
      if (ts > maxTime) maxTime = ts;
   }

   return true;
}

void CppDepends::WriteCache (const std::filesystem::path& file)
{
   std::string writeMe;

   {
      std::stringstream ss;
      ss < file.string();
      ss < static_cast<uint32_t>(dependencies.size());
      for (auto&& dep : dependencies) {
         uint64_t ts = LastWriteTime(dep);
         ss < dep < ts;
         if (ts > maxTime) maxTime = ts;
      }

      writeMe = ss.str();
   }

   const auto ts = std::filesystem::last_write_time(file);

   {
      std::ofstream stream(file.string() + ":CppDepends_Cache5", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
      if (!stream.good()) {
         std::cerr << "Error on writing cache for " << file << std::endl;
         return;
      }

      stream.write(writeMe.c_str(), writeMe.size());
   }

   std::filesystem::last_write_time(file, ts);
}



void CppDepends::ClearIncludePath ()
{
   IncludePaths().clear();
}

void CppDepends::AddIncludePath (const std::filesystem::path& path)
{
   auto p = std::filesystem::canonical(path);
   p.make_preferred();

   if (!std::filesystem::exists(p)) std::cout << "Include-Path " << p << " does not exist. Ignored.";
   else if (!std::filesystem::is_directory(p)) std::cout << "Include-Path " << p << "is invalid. It's not a directory. Ignored";
   else IncludePaths().push_back(p);
}

void CppDepends::PrecompiledHeader (const std::string& prec)
{
   precompiledHeader = prec;
}
