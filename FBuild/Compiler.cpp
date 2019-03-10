/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */


#include "Compiler.h"
#include "CppOutOfDate.h"
#include "ToolChain.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <thread>
#include <mutex>
#include <filesystem>







std::vector<std::string> ActualCompiler::ObjFiles (const std::string& extension)
{
   std::filesystem::path outpath{compiler.ObjDir()};

   std::vector<std::string> result{};

   for (auto&& file : compiler.Files()) {
      std::filesystem::path f{file};
      auto outfile = outpath / f.filename();
      outfile.replace_extension(extension);
      result.push_back(outfile.string());
   }

   return result;
}

std::vector<std::string> ActualCompiler::CompiledObjFiles (const std::string& extension)
{
   std::filesystem::path outpath{compiler.ObjDir()};

   std::vector<std::string> result{};

   for (auto&& file : outOfDate) {
      std::filesystem::path f{file};
      auto outfile = outpath / f.filename();
      outfile.replace_extension(extension);
      result.push_back(outfile.string());
   }

   return result;
}







void ActualCompilerVisualStudio::CheckParams ()
{
   if (compiler.ObjDir().empty()) compiler.ObjDir(compiler.Build());
}

bool ActualCompilerVisualStudio::NeedsRebuild ()
{
   CheckParams();
   outOfDate.clear();

   if (!compiler.DependencyCheck()) {
      outOfDate = compiler.Files();
   }
   else {
      ::CppOutOfDate checker{"obj"};
      checker.OutDir(compiler.ObjDir());
      checker.Threads(compiler.Threads());
      checker.Files(compiler.Files());
      checker.Include(compiler.Includes());
      checker.PrecompiledHeader(compiler.PrecompiledH());
      checker.Go();

      outOfDate = checker.OutOfDate();
   }

   return !outOfDate.empty();
}

void ActualCompilerVisualStudio::DeleteOutOfDateObjectFiles ()
{
   const auto files = CompiledObjFiles();

   for (auto&& file : files) std::filesystem::remove(file);
}

std::string ActualCompilerVisualStudio::CommandLine ()
{
   bool debug = compiler.Build() == "Debug";

   std::string command = "-nologo -c -EHa -GF -Gm- -FC -MP1 -FS -Zc:inline -DWIN32 -DWINDOWS -D_SCL_SECURE_NO_WARNINGS -std:c++latest ";
   

   if (debug) command += "-D_DEBUG ";
   else command += "-DNDEBUG ";

   for (auto&& define : compiler.Defines()) command += "-D" + define + " ";


   if (ToolChain::Platform() == "x86") command += "-arch:SSE2 ";


   if (compiler.CRT() == "Static") command += "-MT";
   else command += "-MD";

   if (debug) command += "d";

   command += " ";


   if (debug) command += "-Od ";
   else command += "-Ox ";


   if (debug) command += "-RTC1 ";


   if (debug) command += "-GS ";
   else command += "-GS- ";


   if (debug) command += "-Zi ";


   for (auto&& include : compiler.Includes()) command += "-I\"" + include + "\" ";


   command += "-W" + std::to_string(compiler.WarnLevel()) + " ";
   if (compiler.WarningAsError()) command += "-WX ";

   for (auto&& disabledWarning : compiler.WarningDisable()) command += "-wd" + std::to_string(disabledWarning) + " ";


   command += compiler.Args() + " ";


   std::filesystem::path out(compiler.ObjDir());
   if (!std::filesystem::exists(out)) std::filesystem::create_directories(out);

   command += "-Fo\"" + out.string() + "/\" ";


   command += "-Fp\"" + out.string() + "\"/PrecompiledHeader.pch ";


   const char* env = std::getenv("FB_COMPILER");
   if (env) command += std::string(env) + " ";

   if (debug) {
      env = std::getenv("FB_COMPILER_DEBUG");
      if (env) command += std::string(env) + " ";
   }
   else {
      env = std::getenv("FB_COMPILER_RELEASE");
      if (env) command += std::string(env) + " ";
   }

   return command;
}

void ActualCompilerVisualStudio::CompilePrecompiledHeaders ()
{
   if (outOfDate.empty()) return;
   if (compiler.PrecompiledCPP().empty()) return;

   std::filesystem::path cpp = std::filesystem::canonical(compiler.PrecompiledCPP());
   cpp.make_preferred();

   auto it = std::find_if(outOfDate.cbegin(), outOfDate.cend(), [&cpp] (const std::string& f) -> bool {
      return std::filesystem::equivalent(cpp, f);
   });

   if (it == outOfDate.cend()) return;

   outOfDate.erase(it);

   std::filesystem::path pch = std::filesystem::path(compiler.ObjDir()) / "PrecompiledHeader.pch";
   if (std::filesystem::exists(pch)) std::filesystem::remove(pch);

   std::string command =  "CL " + CommandLine();
   command += "-Yc\"" + std::filesystem::path{compiler.PrecompiledH()}.filename().string() + "\" ";
   command += cpp.string();

   std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
   int rc = std::system(cmd.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");
}

void ActualCompilerVisualStudio::CompileFiles ()
{
   if (outOfDate.empty()) return;

   std::string commandLine = CommandLine();

   if (compiler.PrecompiledH().size()) {
      commandLine += "-FI\"" + compiler.PrecompiledH() + "\" ";
      commandLine += "-Yu\"" + compiler.PrecompiledH() + "\" ";
   }

   std::vector<std::string> todo = outOfDate;
   size_t errors = 0;
   std::mutex mutex{};

   auto threadFunction = [&] () {
      std::string cpp;
      std::string command;

      for (;;) {
         try {
            {
               std::lock_guard<std::mutex> lock{mutex};
               auto it = todo.rbegin();
               if (it == todo.rend()) break;
               cpp = (*it);
               todo.pop_back();
            }

            command = "cl.exe " + commandLine + "\"" + cpp + "\" ";

            std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
            int rc = std::system(cmd.c_str());
            if (rc != 0) {
               std::lock_guard<std::mutex> lock{mutex};
               ++errors;
            }
         }
         catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            std::lock_guard<std::mutex> lock{mutex};
            ++errors;
         }
         catch (...) {
            std::lock_guard<std::mutex> lock{mutex};
            ++errors;
         }
      }
   };

   size_t threads = compiler.Threads();
   if (!threads) {
      threads = std::thread::hardware_concurrency();
      if (!threads) threads = 2;
   }
   if (threads > todo.size()) threads = todo.size();

   std::vector<std::thread> threadGroup;
   for (size_t i = 0; i < threads; ++i) threadGroup.push_back(std::thread{threadFunction});

   for (auto&& thread : threadGroup) thread.join();

   if (errors) throw std::runtime_error("Compile Error");
}

void ActualCompilerVisualStudio::Compile ()
{
   CheckParams();
   if (!NeedsRebuild()) return;

   std::cout << "\nCompiling (" << ToolChain::ToolChain() << " " << ToolChain::Platform() << ")" << std::endl;

   compiler.DoBeforeCompile();

   DeleteOutOfDateObjectFiles();
   CompilePrecompiledHeaders();
   CompileFiles();
}







void ActualCompilerEmscripten::CheckParams ()
{
   if (compiler.ObjDir().empty()) compiler.ObjDir(compiler.Build());
   if (!std::filesystem::exists(compiler.ObjDir())) std::filesystem::create_directories(compiler.ObjDir());
}

bool ActualCompilerEmscripten::NeedsRebuild ()
{
   CheckParams();
   outOfDate.clear();

   if (!compiler.DependencyCheck()) {
      outOfDate = compiler.Files();
   }
   else {
      ::CppOutOfDate checker{"o"};
      checker.OutDir(compiler.ObjDir());
      checker.Threads(compiler.Threads());
      checker.Files(compiler.Files());
      checker.Include(compiler.Includes());
      checker.PrecompiledHeader(compiler.PrecompiledH());
      checker.Go();

      outOfDate = checker.OutOfDate();
   }

   return !outOfDate.empty();
}

void ActualCompilerEmscripten::DeleteOutOfDateObjectFiles ()
{
   const auto files = CompiledObjFiles();

   for (auto&& file : files) std::filesystem::remove(file);
}

void ActualCompilerEmscripten::CompilePrecompiledHeaders ()
{
   if (outOfDate.empty()) return;
   if (compiler.PrecompiledCPP().empty()) return;

   std::filesystem::path cpp = std::filesystem::canonical(compiler.PrecompiledCPP());
   cpp.make_preferred();

   auto it = std::find_if(outOfDate.cbegin(), outOfDate.cend(), [&cpp] (const std::string& f) -> bool {
      return std::filesystem::equivalent(cpp, f);
   });

   if (it == outOfDate.cend()) return;

   outOfDate.erase(it);

   std::filesystem::path hpp = std::filesystem::canonical(compiler.PrecompiledH());
   hpp.make_preferred();

   std::cout << hpp.string() << std::endl;

   std::string command = "emcc " + CommandLine(true) + "\"" + hpp.string() + "\" -x c++-header -o \"" + hpp.string() + ".pch\" ";

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");

   std::ofstream obj(compiler.ObjDir() + "/" + cpp.filename().replace_extension("o").string());
}

void ActualCompilerEmscripten::CompileFiles ()
{
   if (outOfDate.empty()) return;

   std::string commandLine = CommandLine(false);
   if (compiler.PrecompiledH().size()) {
      std::filesystem::path hpp = std::filesystem::canonical(compiler.PrecompiledH());
      hpp.make_preferred();

      commandLine += " -include \"" + hpp.string() + "\" ";
   }

   std::vector<std::string> todo = outOfDate;
   size_t errors = 0;
   std::mutex mutex{};

   auto threadFunction = [&] () {
      std::string cpp;
      std::string command;

      for (;;) {
         try {
            {
               std::lock_guard<std::mutex> lock{mutex};
               auto it = todo.rbegin();
               if (it == todo.rend()) break;
               cpp = (*it);
               todo.pop_back();

               std::cout << cpp << std::endl;
            }

            command = "emcc " + commandLine + "\"" + cpp + "\" ";

            int rc = std::system(command.c_str());
            if (rc != 0) {
               std::lock_guard<std::mutex> lock{mutex};
               ++errors;
            }
         }
         catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            std::lock_guard<std::mutex> lock{mutex};
            ++errors;
         }
         catch (...) {
            std::lock_guard<std::mutex> lock{mutex};
            ++errors;
         }
      }
   };

   size_t threads = compiler.Threads();
   if (!threads) {
      threads = std::thread::hardware_concurrency();
      if (!threads) threads = 2;
   }
   if (threads > todo.size()) threads = todo.size();

   std::vector<std::thread> threadGroup;
   for (size_t i = 0; i < threads; ++i) threadGroup.push_back(std::thread{threadFunction});

   for (auto&& thread : threadGroup) thread.join();

   if (errors) throw std::runtime_error("Compile Error");
}

std::string ActualCompilerEmscripten::CommandLine (bool omitObjDir)
{
   bool debug = compiler.Build() == "Debug";

   std::string command = "-c -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 --memory-init-file 0 -std=c++1z ";

   command += "-fdiagnostics-format=msvc -Wno-invalid-source-encoding ";

   if (debug) command += "-g -O1 -D_DEBUG ";
   else command += "-O3 -DNDEBUG ";


   for (auto&& define : compiler.Defines()) command += "-D" + define + " ";


   for (auto&& include : compiler.Includes()) command += "-I\"" + include + "\" ";


   if (compiler.WarnLevel() == 0) command += "-w ";
   else if (compiler.WarnLevel() == 4) {
      if (compiler.WarningAsError()) command += "-pedantic-errors ";
      else command += "-pedantic ";
   }

   if (compiler.WarningAsError()) command += "-Werror ";

   for (auto&& disabledWarning : compiler.WarningDisable()) command += "-Wno-" + std::to_string(disabledWarning) + " ";


   command += compiler.Args() + " ";


   if (!omitObjDir) command += "-o " + compiler.ObjDir() + "/ ";


   return command;
}

void ActualCompilerEmscripten::Compile ()
{
   CheckParams();
   if (!NeedsRebuild()) return;

   std::cout << "\nCompiling (" << ToolChain::ToolChain() << ")" << std::endl;

   compiler.DoBeforeCompile();

   DeleteOutOfDateObjectFiles();
   CompilePrecompiledHeaders();
   CompileFiles();
}








void Compiler::Compile ()
{
   const auto toolChain = ToolChain::ToolChain();
   if (toolChain.substr(0, 4) == "MSVC") actualCompiler.reset(new ActualCompilerVisualStudio{*this});
   else if (toolChain == "EMSCRIPTEN") actualCompiler.reset(new ActualCompilerEmscripten{*this});
   else throw std::runtime_error("Unbekannte Toolchain: " + toolChain);

   actualCompiler->Compile();
}

