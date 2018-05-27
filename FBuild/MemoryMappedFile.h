/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <memory>
#include <filesystem>
#include <string>


class MemoryMappedFile {
public:
   enum class Mode { ReadOnly, ReadWrite };

   explicit MemoryMappedFile(const std::filesystem::path& file, uint64_t size = 0, Mode mode = Mode::ReadOnly); // In ReadOnly mode, the size is ignored. In ReadOnly it's always the actual file size (= no resizing of the file, because resizing would be a write operation).

   uint64_t Size() const { return size_; }

   char* Memory ()             { return memoryMapping_.get(); }
   const char* Memory () const { return memoryMapping_.get(); }

   char& operator[] (uint64_t idx)      { return memoryMapping_.get()[idx]; }
   char operator[] (uint64_t idx) const { return memoryMapping_.get()[idx]; }

   char* Begin () { return memoryMapping_.get(); }
   const char* Begin () const { return memoryMapping_.get(); }
   const char* CBegin () const { return memoryMapping_.get(); }

   char* End () { return memoryMapping_.get() + size_; }
   const char* End () const { return memoryMapping_.get() + size_; }
   const char* CEnd () const { return memoryMapping_.get() + size_; }

   char* begin () { return memoryMapping_.get(); }
   const char* begin () const { return memoryMapping_.get(); }
   const char* cbegin () const { return memoryMapping_.get(); }

   char* end () { return memoryMapping_.get() + size_; }
   const char* end () const { return memoryMapping_.get() + size_; }
   const char* cend () const { return memoryMapping_.get() + size_; }

private:
   std::filesystem::path  file_;
   uint64_t               size_;
   Mode                   mode_;
   std::shared_ptr<void>  fileHandle_;
   std::shared_ptr<void>  fileMappingHandle_;
   std::shared_ptr<char>  memoryMapping_;


   std::string ErrorMessage() const;
   void OpenFile();
   void CreateFMapping();
   void CreateMemoryMapping();
};

