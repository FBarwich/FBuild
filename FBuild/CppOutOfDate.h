/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include "CppDepends.h"

#include <algorithm>
#include <ctime>
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>

#include <boost/thread.hpp>


class CppOutOfDate {

   boost::thread_group threadGroup;
   std::mutex          filesMutex;
   std::mutex          outOfDateMutex;
   size_t              current;
   uint64_t            scriptTime;
   std::string         objectFileExtension;

   std::string              outdir;
   bool                     ignoreCache;
   size_t                   threads;
   std::vector<std::string> files;
   std::vector<std::string> outOfDate;

   inline uint64_t LastWriteTime (const std::filesystem::path& file)
   {
      return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(file).time_since_epoch()).count();
   }

   bool GetFile (std::filesystem::path& result)
   {
      std::lock_guard lock(filesMutex);
      if (current == files.size()) return false;

      result = files[current];
      ++current;
      return true;
   }

   bool Done ()
   {
      std::lock_guard lock(filesMutex);
      return current == files.size();
   }

   void AddOutOfDate (const std::string& file)
   {
      std::lock_guard lock(outOfDateMutex);
      outOfDate.push_back(file);
   }

   void Thread ()
   {
      std::filesystem::path objdir(outdir);
      std::filesystem::path file;

      for (;;) {
         if (!GetFile(file)) break;
         CppDepends dep(file, ignoreCache);

         auto obj = objdir / file.filename();
         obj.replace_extension(objectFileExtension);

         if (!std::filesystem::exists(obj)) AddOutOfDate(file.string());
         else if (LastWriteTime(obj) < dep.MaxTime()) AddOutOfDate(file.string());
         else if (LastWriteTime(obj) < scriptTime) AddOutOfDate(file.string());
      }
   }


public:
   CppOutOfDate (const std::string& objectFileExtension) : objectFileExtension{"." + objectFileExtension}
   {
      current = 0;
      ignoreCache = false;
      threads = 0;
      scriptTime = LastWriteTime("FBuild.js");
      files.reserve(1000);

      CppDepends::ClearIncludePath();
      CppDepends::PrecompiledHeader("");
   }

   void OutDir (std::string v)                      { outdir = std::move(v); }
   void IgnoreCache (bool v)                        { ignoreCache = v; }
   void Threads (size_t v)                          { threads = v; }
   void AddIncludePath (const std::string& path)    { CppDepends::AddIncludePath(path); }
   void Files (std::vector<std::string>&& v)        { files = std::move(v); }
   void Files (const std::vector<std::string>& v)   { std::copy(v.begin(), v.end(), std::back_inserter(files)); }
   void Include (const std::vector<std::string>& v) { std::for_each(v.begin(), v.end(), [] (const std::string& s) { CppDepends::AddIncludePath(s); }); }
   void PrecompiledHeader (const std::string& v)    { CppDepends::PrecompiledHeader(v); }

   void Go ()
   {
      if (outdir.empty()) throw std::runtime_error("Missing 'Outdir'");

      size_t cpus = boost::thread::hardware_concurrency();
      if (!cpus) cpus = 2;
      if (threads) cpus = threads;

      for (size_t i = 0; i < cpus; ++i) {
         threadGroup.add_thread(new boost::thread(std::bind(&CppOutOfDate::Thread, this)));
      }

      for (;;) {
         if (Done()) break;
         boost::thread::yield();
      }

      threadGroup.join_all();
   }

   const std::vector<std::string>& OutOfDate () const { return outOfDate; }
};
