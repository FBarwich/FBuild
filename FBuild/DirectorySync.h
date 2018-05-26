#pragma once

#include <filesystem>

class DirectorySync {
   std::filesystem::path source;
   std::filesystem::path dest;

   void CheckSourceAndDest();
   void Copy();
   void Copy(const std::filesystem::path& sourceFile, const std::filesystem::path& destFile);
   bool NeedsCopy(const std::filesystem::path& sourceFile, const std::filesystem::path& destFile);
   void Delete();

public:
   DirectorySync(const std::filesystem::path& source, const std::filesystem::path& dest) : source(source), dest(dest) { }

   void Go();
};

