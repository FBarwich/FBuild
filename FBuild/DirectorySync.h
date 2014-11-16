#pragma once

#include <boost/filesystem.hpp>

class DirectorySync {
   boost::filesystem::path source;
   boost::filesystem::path dest;

   void CheckSourceAndDest();
   void Copy();
   void Copy(const boost::filesystem::path& sourceFile, const boost::filesystem::path& destFile);
   bool NeedsCopy(const boost::filesystem::path& sourceFile, const boost::filesystem::path& destFile);
   void Delete();

public:
   DirectorySync(const boost::filesystem::path& source, const boost::filesystem::path& dest) : source(source), dest(dest) { }

   void Go();
};

