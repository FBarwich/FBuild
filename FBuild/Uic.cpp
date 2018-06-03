/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Uic.h"

#include <filesystem>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>

Uic::Uic(const std::string& qtBinDirectory)
{
   auto exe = std::filesystem::path{qtBinDirectory} / "uic.exe";
   if (!std::filesystem::exists(exe)) throw std::runtime_error{exe.string() + " doesn't exist"};
   exe.make_preferred();
   uicExe_ = exe.string();
}

void Uic::Compile()
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

               const auto full = std::filesystem::canonical(outFile);
               outFiles_.emplace_back(full.string());
            }

            if (!NeedsRebuild(file, outFile)) continue;

            std::cout << "Moc: " << file << std::endl;

            command = uicExe_ + " -o \"" + outFile + "\" ";
            command += file;
            int rc = std::system(command.c_str());
            if (rc != 0) ++errors;
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

   if (errors) throw std::runtime_error("UIC Error");
}

std::string Uic::OutFile(const std::string& inFile) const
{
   std::filesystem::path fn{inFile};
   fn.replace_extension(".h");
   std::string filename = prefix_ + fn.filename().string();

   const auto outfile = std::filesystem::path{outDir_} / filename;
   return outfile.string();
}

bool Uic::NeedsRebuild(const std::string& inFile, const std::string& outFile) const
{
   if (!dependencyCheck_) return true;
   if (!std::filesystem::exists(outFile)) return true;

   return std::filesystem::last_write_time(outFile) < std::filesystem::last_write_time(inFile);
}
