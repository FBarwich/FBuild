/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include "CppDepends.h"

#include <boost/thread.hpp>


class CppOutOfDate {
   boost::thread_group threadGroup;
   boost::mutex        filesMutex;
   boost::mutex        outOfDateMutex;
   size_t              current;

   std::string              outdir;
   bool                     ignoreCache;
   size_t                   threads;
   std::vector<std::string> files;
   std::vector<std::string> outOfDate;

   bool GetFile (boost::filesystem::path& result)
   {
      boost::lock_guard<boost::mutex> lock(filesMutex);
      if (current == files.size()) return false;

      result = files[current];
      ++current;
      return true;
   }

   bool Done ()
   {
      boost::lock_guard<boost::mutex> lock(filesMutex);
      return current == files.size();
   }

   void AddOutOfDate (const std::string& file)
   {
      boost::lock_guard<boost::mutex> lock(outOfDateMutex);
      outOfDate.push_back(file);
   }

   void Thread ()
   {
      boost::filesystem::path objdir(outdir);
      boost::filesystem::path file;

      for (;;) {
         if (!GetFile(file)) break;
         CppDepends dep(file, ignoreCache);

         auto obj = objdir / file.filename();
         obj.replace_extension(".obj");

         if (!boost::filesystem::exists(obj)) AddOutOfDate(file.string());
         else if (boost::filesystem::last_write_time(obj) < dep.MaxTime()) AddOutOfDate(file.string());
      }
   }


public:
   CppOutOfDate ()
   {
      current = 0;
      ignoreCache = false;
      threads = 0;
      files.reserve(1000);

      CppDepends::ClearIncludePath();
   }

   void OutDir (std::string v)                   { outdir = std::move(v); }
   void IgnoreCache (bool v)                     { ignoreCache = v; }
   void Threads (size_t v)                       { threads = v; }
   void AddIncludePath (const std::string& path) { CppDepends::AddIncludePath(path); }
   void AddFile (std::string file)               { files.push_back(std::move(file)); }

   void Go ()
   {
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
