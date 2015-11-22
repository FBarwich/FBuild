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

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>






std::vector<std::string> ActualCompiler::ObjFiles (const std::string& extension)
{
   boost::filesystem::path outpath{compiler.ObjDir()};

   std::vector<std::string> result{};

   for (auto&& file : compiler.Files()) {
      boost::filesystem::path f{file};
      auto outfile = outpath / f.filename();
      outfile.replace_extension(extension);
      result.push_back(outfile.string());
   }

   return result;
}

std::vector<std::string> ActualCompiler::CompiledObjFiles (const std::string& extension)
{
   boost::filesystem::path outpath{compiler.ObjDir()};

   std::vector<std::string> result{};

   for (auto&& file : outOfDate) {
      boost::filesystem::path f {file};
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

   for (auto&& file : files) boost::filesystem::remove(file);
}

std::string ActualCompilerVisualStudio::CommandLine ()
{
   bool debug = compiler.Build() == "Debug";

   std::string command = "-nologo -c -EHa -GF -Gm- -FC -MP1 -FS -DWIN32 -DWINDOWS -D_SCL_SECURE_NO_WARNINGS ";
   

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


   command += "-W" + boost::lexical_cast<std::string>(compiler.WarnLevel()) + " ";
   if (compiler.WarningAsError()) command += "-WX ";

   for (auto&& disabledWarning : compiler.WarningDisable()) command += "-wd" + boost::lexical_cast<std::string>(disabledWarning) + " ";


   command += compiler.Args() + " ";


   boost::filesystem::path out(compiler.ObjDir());
   if (!boost::filesystem::exists(out)) boost::filesystem::create_directories(out);

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

   boost::filesystem::path cpp = boost::filesystem::canonical(compiler.PrecompiledCPP());
   cpp.make_preferred();

   auto it = std::find_if(outOfDate.cbegin(), outOfDate.cend(), [&cpp] (const std::string& f) -> bool {
      return boost::filesystem::equivalent(cpp, f);
   });

   if (it == outOfDate.cend()) return;

   outOfDate.erase(it);

   boost::filesystem::path pch = boost::filesystem::path(compiler.ObjDir()) / "PrecompiledHeader.pch";
   if (boost::filesystem::exists(pch)) boost::filesystem::remove(pch);

   std::string command =  "CL " + CommandLine();
   command += "-Yc\"" + boost::filesystem::path{compiler.PrecompiledH()}.filename().string() + "\" ";
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
   if (!boost::filesystem::exists(compiler.ObjDir())) boost::filesystem::create_directories(compiler.ObjDir());
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

   for (auto&& file : files) boost::filesystem::remove(file);
}

void ActualCompilerEmscripten::CompilePrecompiledHeaders ()
{
   if (outOfDate.empty()) return;
   if (compiler.PrecompiledCPP().empty()) return;

   boost::filesystem::path cpp = boost::filesystem::canonical(compiler.PrecompiledCPP());
   cpp.make_preferred();

   auto it = std::find_if(outOfDate.cbegin(), outOfDate.cend(), [&cpp] (const std::string& f) -> bool {
      return boost::filesystem::equivalent(cpp, f);
   });

   if (it == outOfDate.cend()) return;

   outOfDate.erase(it);

   boost::filesystem::path hpp = boost::filesystem::canonical(compiler.PrecompiledH());
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
      boost::filesystem::path hpp = boost::filesystem::canonical(compiler.PrecompiledH());
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

   std::string command = "-c -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 --memory-init-file 0 -std=c++14 ";

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

   for (auto&& disabledWarning : compiler.WarningDisable()) command += "-Wno-" + boost::lexical_cast<std::string>(disabledWarning) + " ";


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

