#include "DirectorySync.h"

#include <iostream>
#include <fstream>

void DirectorySync::CheckSourceAndDest()
{
   if (!boost::filesystem::exists(source)) throw std::runtime_error("Directory '" + source.string() + "' does not exist");
   if (!boost::filesystem::is_directory(source)) throw std::runtime_error("'" + source.string() + "' is not a directory");

   if (!boost::filesystem::exists(dest)) {
      std::cout << "Creating destination directory '" + dest.string() << "'\n";
      boost::filesystem::create_directories(dest);
   }
   if (!boost::filesystem::is_directory(dest)) throw std::runtime_error("'" + dest.string() + "' is not a directory");
}

void DirectorySync::Copy()
{
   std::for_each(boost::filesystem::directory_iterator(source), boost::filesystem::directory_iterator(), [&] (const boost::filesystem::directory_entry& entry) {
      if (boost::filesystem::is_directory(entry.path())) {
         DirectorySync(entry.path(), dest / entry.path().filename()).Go();
      }
      else {
         Copy(entry.path(), dest / entry.path().filename());
      }
   });
}

void DirectorySync::Copy(const boost::filesystem::path& sourceFile, const boost::filesystem::path& destFile)
{
   if (!NeedsCopy(sourceFile, destFile)) return;

   std::cout << "Copy " << sourceFile << " to " << destFile << "...";
   
   {
      std::ifstream in(sourceFile.string(), std::ifstream::binary);
      std::ofstream out(destFile.string(), std::ofstream::trunc | std::ofstream::binary);
      out << in.rdbuf();
   }

   std::time_t sourceTime = boost::filesystem::last_write_time(sourceFile);
   boost::filesystem::last_write_time(destFile, sourceTime);

   std::cout << " OK\n";
}

bool DirectorySync::NeedsCopy(const boost::filesystem::path& sourceFile, const boost::filesystem::path& destFile)
{
   if (!boost::filesystem::exists(destFile)) return true;

   std::time_t sourceTime = boost::filesystem::last_write_time(sourceFile);
   std::time_t destTime = boost::filesystem::last_write_time(destFile);

   return destTime < sourceTime;
}

void DirectorySync::Delete()
{
   std::for_each(boost::filesystem::directory_iterator(dest), boost::filesystem::directory_iterator(), [&] (const boost::filesystem::directory_entry& entry) {
      if (boost::filesystem::is_directory(entry.path())) {
         if (!boost::filesystem::exists(source / entry.path().filename())) {
            std::cout << "Deleting Directory " << entry.path().string() << "...";

            boost::filesystem::remove_all(entry.path());

            std::cout << "OK\n";
         }
      }
      else {
         if (!boost::filesystem::exists(source / entry.path().filename())) {
            std::cout << "Deleting File " << entry.path().string() << "...";

            boost::filesystem::remove(entry.path());

            std::cout << "OK\n";
         }
      }
   });
}

void DirectorySync::Go()
{
   CheckSourceAndDest();
   Copy();
   Delete();
}

