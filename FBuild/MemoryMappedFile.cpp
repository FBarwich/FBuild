#include "Precompiled.h"

#include "MemoryMappedFile.h"

#define NOMINMAX
#include <Windows.h>


MemoryMappedFile::MemoryMappedFile(const std::filesystem::path& file, uint64_t size, Mode mode) : file_{file}, size_{size}, mode_{mode}
{
   if (mode == Mode::ReadOnly) size_ = 0;

   OpenFile();
   CreateFMapping();
   CreateMemoryMapping();
}

std::string MemoryMappedFile::ErrorMessage() const
{
   const auto lastError = ::GetLastError();

   std::string result;
   TCHAR* message{};

   const auto count = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, lastError, 0, message, 0, nullptr);
   if (count) {
      result = message;
      LocalFree(message);
   }
   else {
      result = "Unknown Error. FormatMessage() has nothing for us...";
   }

   result += " (" + std::to_string(lastError) + ")";
   return result;
}

void MemoryMappedFile::OpenFile()
{
   DWORD access = mode_ == Mode::ReadOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
   DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
   DWORD open = mode_ == Mode::ReadOnly ? OPEN_EXISTING : OPEN_ALWAYS;

   void* handle = ::CreateFile(file_.string().c_str(), access, share, NULL, open, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
   if (handle == INVALID_HANDLE_VALUE) throw std::runtime_error{"Error opening file " + file_.string() + "\n" + ErrorMessage()};
   fileHandle_.reset(handle, ::CloseHandle);
}

void MemoryMappedFile::CreateFMapping()
{
   if (size_ == 0) {
      LARGE_INTEGER nul;
      nul.QuadPart = 0;

      LARGE_INTEGER pos;

      if (!::SetFilePointerEx(fileHandle_.get(), nul, &pos, FILE_END)) throw std::runtime_error{"Unable to determine the file size (SetFilePointerEx) for " + file_.string() + "\n" + ErrorMessage()};

      size_ = pos.QuadPart;
   }
   else {
      LARGE_INTEGER newPos;
      newPos.QuadPart = size_;

      if (!::SetFilePointerEx(fileHandle_.get(), newPos, nullptr, FILE_BEGIN)) throw std::runtime_error {"Unable to set file pointer (SetFilePointerEx) for " + file_.string() + "\n" + ErrorMessage()};
      if (!::SetEndOfFile(fileHandle_.get())) throw std::runtime_error {"Unable to truncate file (SetEndOfFile) for " + file_.string() + "\n" + ErrorMessage()};
   }

   if (size_ == 0) throw std::runtime_error{"Can't map the empty file " + file_.string() + " into memory"};

   DWORD flags = mode_ == Mode::ReadOnly ? PAGE_READONLY : PAGE_READWRITE;
   LARGE_INTEGER size;
   size.QuadPart = size_;

   void* handle = ::CreateFileMapping(fileHandle_.get(), 0, flags, size.HighPart, size.LowPart, 0);
   if (!handle) throw std::runtime_error{"Couldn't create a file mapping for " + file_.string() + "\n" + ErrorMessage()};
   fileMappingHandle_.reset(handle, ::CloseHandle);
}

void MemoryMappedFile::CreateMemoryMapping()
{
   DWORD flags = mode_ == Mode::ReadOnly ? FILE_MAP_READ : FILE_MAP_WRITE;

   char* ptr = (char*)::MapViewOfFile(fileMappingHandle_.get(), flags, 0, 0, 0);
   if (!ptr) throw std::runtime_error{"Couldn't map the file " + file_.string() + " into memory\n" + ErrorMessage()};
   memoryMapping_.reset(ptr, ::UnmapViewOfFile);
}
