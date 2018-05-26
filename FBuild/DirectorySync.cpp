#include "DirectorySync.h"

#include <iostream>
#include <fstream>

void DirectorySync::CheckSourceAndDest()
{
   if (!std::filesystem::exists(source)) throw std::runtime_error("Directory '" + source.string() + "' does not exist");
   if (!std::filesystem::is_directory(source)) throw std::runtime_error("'" + source.string() + "' is not a directory");

   if (!std::filesystem::exists(dest)) {
      std::cout << "Creating destination directory '" + dest.string() << "'\n";
      std::filesystem::create_directories(dest);
   }
   if (!std::filesystem::is_directory(dest)) throw std::runtime_error("'" + dest.string() + "' is not a directory");
}

void DirectorySync::Copy()
{
   std::for_each(std::filesystem::directory_iterator(source), std::filesystem::directory_iterator(), [&] (const std::filesystem::directory_entry& entry) {
      if (std::filesystem::is_directory(entry.path())) {
         DirectorySync(entry.path(), dest / entry.path().filename()).Go();
      }
      else {
         Copy(entry.path(), dest / entry.path().filename());
      }
   });
}

void DirectorySync::Copy(const std::filesystem::path& sourceFile, const std::filesystem::path& destFile)
{
   if (!NeedsCopy(sourceFile, destFile)) return;

   std::cout << "Copy " << sourceFile << " to " << destFile << "...";
   
   {
      std::ifstream in(sourceFile.string(), std::ifstream::binary);
      std::ofstream out(destFile.string(), std::ofstream::trunc | std::ofstream::binary);
      out << in.rdbuf();
   }

   const auto sourceTime = std::filesystem::last_write_time(sourceFile);
   std::filesystem::last_write_time(destFile, sourceTime);

   std::cout << " OK\n";
}

bool DirectorySync::NeedsCopy(const std::filesystem::path& sourceFile, const std::filesystem::path& destFile)
{
   if (!std::filesystem::exists(destFile)) return true;

   const auto sourceTime = std::filesystem::last_write_time(sourceFile);
   const auto destTime = std::filesystem::last_write_time(destFile);

   return destTime < sourceTime;
}

void DirectorySync::Delete()
{
   std::for_each(std::filesystem::directory_iterator(dest), std::filesystem::directory_iterator(), [&] (const std::filesystem::directory_entry& entry) {
      if (std::filesystem::is_directory(entry.path())) {
         if (!std::filesystem::exists(source / entry.path().filename())) {
            std::cout << "Deleting Directory " << entry.path().string() << "...";

            std::filesystem::remove_all(entry.path());

            std::cout << "OK\n";
         }
      }
      else {
         if (!std::filesystem::exists(source / entry.path().filename())) {
            std::cout << "Deleting File " << entry.path().string() << "...";

            std::filesystem::remove(entry.path());

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
