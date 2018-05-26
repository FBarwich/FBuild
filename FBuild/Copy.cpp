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


static std::vector<std::filesystem::path> CollectSourceFiles (const std::string& p)
{
   std::vector<std::filesystem::path> result;

   if (p.find_first_of("*?") == std::string::npos && std::filesystem::exists(p)) {
      if (std::filesystem::is_regular_file(p)) result.push_back(p);
      else {
         for (auto&& entry : std::filesystem::directory_iterator{p}) {
            if (std::filesystem::is_regular_file(entry.path())) result.push_back(entry.path());
         }
      }
   }
   else {
      std::filesystem::path path(p);
      std::string pattern = path.filename().string();
      path.remove_filename();

      for (auto&& entry : std::filesystem::directory_iterator{path}) {
         if (std::filesystem::is_regular_file(entry.path())) {
            if (::PathMatchSpec(entry.path().filename().string().c_str(), pattern.c_str())) {
               result.push_back(entry.path());
            }
         }
      }
   }

   return result;
}

void Copy::DoFile (std::filesystem::path sourceFile, std::filesystem::path destFile)
{
   sourceFile.make_preferred();
   destFile.make_preferred();

   if (std::filesystem::exists(destFile) && !ignoreTimestamp) {
      const auto sourceTime = std::filesystem::last_write_time(sourceFile);
      const auto destTime = std::filesystem::last_write_time(destFile);

      if (destTime >= sourceTime) return;
   }

   std::cout << "Copy " << sourceFile << " to " << destFile << "...";

   {
      std::ifstream in(sourceFile.string(), std::ifstream::binary);
      std::ofstream out(destFile.string(), std::ofstream::trunc | std::ofstream::binary);
      out << in.rdbuf();
   }

   const auto sourceTime = std::filesystem::last_write_time(sourceFile);
   std::filesystem::last_write_time(destFile, sourceTime);

   ++copied;

   std::cout << " OK\n";
}

int Copy::Go ()
{
   CheckParams();

   std::filesystem::create_directories(dest);

   auto sourceFiles = CollectSourceFiles(source);

   std::filesystem::path destPath(dest);

   std::for_each(sourceFiles.begin(), sourceFiles.end(), [&] (const std::filesystem::path& file) {
      this->DoFile(file, destPath / file.filename());
   });

   return copied;
}

static bool NeedsCopy (std::filesystem::path source, std::filesystem::path dest)
{
   source.make_preferred();
   dest.make_preferred();

   if (std::filesystem::exists(dest)) {
      const auto sourceTime = std::filesystem::last_write_time(source);
      const auto destTime = std::filesystem::last_write_time(dest);

      if (destTime >= sourceTime) return false;
   }

   return true;
}

bool Copy::NeedsCopy () const
{
   CheckParams();

   if (ignoreTimestamp) return true;

   auto sourceFiles = CollectSourceFiles(source);
   std::filesystem::path destPath(dest);

   for (size_t i = 0; i < sourceFiles.size(); ++i) {
      const std::filesystem::path& file = sourceFiles[i];
      if (::NeedsCopy(file, destPath / file.filename())) return true;
   }

   return false;
}

void Copy::CheckParams () const
{
   if (source.empty()) throw std::runtime_error("Missing source for Copy");
   if (dest.empty()) throw std::runtime_error("Missing dest for Copy");
}
