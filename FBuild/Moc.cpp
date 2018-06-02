/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Moc.h"
#include "MemoryMappedFile.h"

#include <filesystem>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <fstream>

Moc::Moc (const std::string& qtBinDirectory)
{
   auto exe = std::filesystem::path{qtBinDirectory} / "moc.exe";
   if (!std::filesystem::exists(exe)) throw std::runtime_error{exe.string() + " doesn't exist"};
   exe.make_preferred();
   mocExe_ = exe.string();
}

void Moc::Compile()
{
   if (files_.empty()) return;
   if (outDir_.empty()) throw std::runtime_error("Mising 'Outdir'");

   if (!std::filesystem::exists(outDir_)) std::filesystem::create_directories(outDir_);

   std::vector<std::string> todo{files_};
   std::atomic<uint32_t> errors{0};
   std::mutex mutex{};

   const auto threadFunction = [&] () {
      std::string file{};
      std::string outFile{};
      std::string command{};

      for (;;) {
         try {
            {
               std::lock_guard guard{mutex};
               const auto it = todo.rbegin();
               if (it == todo.rend()) break;
               file = (*it);
               todo.pop_back();

               outFile = OutFile(file);
               outFiles_.emplace_back(outFile);
            }

            if (!NeedsRebuild(file, outFile)) continue;

            if (!NeedsMoc(file)) {
               std::ofstream emptyFile{outFile, std::ofstream::trunc};
            }
            else {
               std::cout << "Moc: " << file << std::endl;

               command = mocExe_ + " -o \"" + outFile + "\" ";
               command += file;
               int rc = std::system(command.c_str());
               if (rc != 0) ++errors;
            }

         }
         catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            ++errors;
         }
         catch (...) {
            ++errors;
         }
      }
   };

   size_t threadCount = std::thread::hardware_concurrency();
   if (!threadCount) threadCount = 2;
   if (threadCount > todo.size()) threadCount = todo.size();

   std::vector<std::thread> threads;
   for (size_t i = 0; i < threadCount; ++i) threads.emplace_back(std::thread{threadFunction});

   for (auto&& thread : threads) thread.join();

   if (errors) throw std::runtime_error("Moc Error");

   // We're using an empty *.cpp to mark files for which moc isn't needed. This way we can skip them by timestamp without having to look into every No-Moc-File to search for Q_OBJECT, just to skip them anyway.
   std::vector<std::string> tmp{};
   tmp.swap(outFiles_);
   for (auto&& file : tmp) {
      if (std::filesystem::file_size(file) != 0) {
         outFiles_.emplace_back(std::filesystem::canonical(file).string());
      }
   }
}

std::string Moc::OutFile(const std::string& inFile) const
{
   std::filesystem::path fn{inFile};
   fn.replace_extension(".cpp");
   std::string filename = prefix_ + fn.filename().string();

   const auto outfile = std::filesystem::path{outDir_} / filename;
   return outfile.string();
}

bool Moc::NeedsRebuild(const std::string& inFile, const std::string& outFile) const
{
   if (!dependencyCheck_) return true;
   if (!std::filesystem::exists(outFile)) return true;

   return std::filesystem::last_write_time(outFile) < std::filesystem::last_write_time(inFile);
}

bool Moc::NeedsMoc(const std::string& file) const
{
   static const std::string searchString{"Q_OBJECT"};

   const MemoryMappedFile mmf{file};
   return std::search(begin(mmf), end(mmf), begin(searchString), end(searchString)) != end(mmf);
}
