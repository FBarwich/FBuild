#include "Copy.h"

#include <iostream>
#include <fstream>

#include <Shlwapi.h>


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

void Copy::DoFile (boost::filesystem::path source, boost::filesystem::path dest)
{
   source.make_preferred();
   dest.make_preferred();

   if (!ignoreTimestamp) {
      std::time_t sourceTime = boost::filesystem::last_write_time(source);
      std::time_t destTime = boost::filesystem::last_write_time(dest);

      if (destTime <= sourceTime) return;
   }

   std::cout << "Copy " << source << " to " << dest << "...";

   {
      std::ifstream in(source.string(), std::ifstream::binary);
      std::ofstream out(dest.string(), std::ofstream::trunc | std::ofstream::binary);
      out << in.rdbuf();
   }

   std::time_t sourceTime = boost::filesystem::last_write_time(source);
   boost::filesystem::last_write_time(dest, sourceTime);

   std::cout << " OK\n";
}

int Copy::Go ()
{
   auto sourceFiles = CollectSourceFiles(source);

   boost::filesystem::path destPath(dest);

   std::for_each(sourceFiles.begin(), sourceFiles.end(), [&] (const boost::filesystem::path& file) {
      this->DoFile(file, destPath / file.filename());
   });

   return copied;
}
