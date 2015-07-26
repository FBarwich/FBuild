/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Copy.h"

#include <iostream>
#include <fstream>

#include <Shlwapi.h>

#include "JavaScript.h"


static std::vector<boost::filesystem::path> CollectSourceFiles (const std::string& p)
{
   std::vector<boost::filesystem::path> result;

   if (boost::filesystem::exists(p)) {
      if (boost::filesystem::is_regular_file(p)) result.push_back(p);
      else {
         std::for_each(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), [&] (const boost::filesystem::directory_entry& entry) {
            if (boost::filesystem::is_regular_file(entry.path())) {
               result.push_back(entry.path());
            }
         });
      }
   }
   else {
      boost::filesystem::path path(p);
      std::string pattern = path.filename().string();
      path.remove_filename();

      std::for_each(boost::filesystem::directory_iterator(path), boost::filesystem::directory_iterator(), [&] (const boost::filesystem::directory_entry& entry) {
         if (boost::filesystem::is_regular_file(entry.path())) {
            if (PathMatchSpec(entry.path().filename().string().c_str(), pattern.c_str())) {
               result.push_back(entry.path());
            }
         }
      });
   }

   return result;
}

void Copy::DoFile (boost::filesystem::path sourceFile, boost::filesystem::path destFile)
{
   sourceFile.make_preferred();
   destFile.make_preferred();

   if (boost::filesystem::exists(destFile) && !ignoreTimestamp) {
      std::time_t sourceTime = boost::filesystem::last_write_time(sourceFile);
      std::time_t destTime = boost::filesystem::last_write_time(destFile);

      if (destTime >= sourceTime) return;
   }

   std::cout << "Copy " << sourceFile << " to " << destFile << "...";

   {
      std::ifstream in(sourceFile.string(), std::ifstream::binary);
      std::ofstream out(destFile.string(), std::ofstream::trunc | std::ofstream::binary);
      out << in.rdbuf();
   }

   std::time_t sourceTime = boost::filesystem::last_write_time(sourceFile);
   boost::filesystem::last_write_time(destFile, sourceTime);

   ++copied;

   std::cout << " OK\n";
}

int Copy::Go ()
{
   CheckParams();

   boost::filesystem::create_directories(dest);

   auto sourceFiles = CollectSourceFiles(source);

   boost::filesystem::path destPath(dest);

   std::for_each(sourceFiles.begin(), sourceFiles.end(), [&] (const boost::filesystem::path& file) {
      this->DoFile(file, destPath / file.filename());
   });

   return copied;
}

static bool NeedsCopy (boost::filesystem::path source, boost::filesystem::path dest)
{
   source.make_preferred();
   dest.make_preferred();

   if (boost::filesystem::exists(dest)) {
      std::time_t sourceTime = boost::filesystem::last_write_time(source);
      std::time_t destTime = boost::filesystem::last_write_time(dest);

      if (destTime >= sourceTime) return false;
   }

   return true;
}

bool Copy::NeedsCopy () const
{
   CheckParams();

   if (ignoreTimestamp) return true;

   auto sourceFiles = CollectSourceFiles(source);
   boost::filesystem::path destPath(dest);

   for (size_t i = 0; i < sourceFiles.size(); ++i) {
      const boost::filesystem::path& file = sourceFiles[i];
      if (::NeedsCopy(file, destPath / file.filename())) return true;
   }

   return false;
}

void Copy::CheckParams () const
{
   if (source.empty()) throw std::runtime_error("Missing source for Copy");
   if (dest.empty()) throw std::runtime_error("Missing dest for Copy");
}
