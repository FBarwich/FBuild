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

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>


inline std::string MP (int threads)
{
   std::string mp = "-MP";
   if (threads) mp += boost::lexical_cast<std::string>(threads);
   return mp + " ";
}

inline std::string Fo (const std::string& outDir)
{
   boost::filesystem::path out(outDir);
   if (!boost::filesystem::exists(out)) boost::filesystem::create_directories(out);

   return "-Fo\"" + out.string() + "/\" ";
}

inline std::string Fp (const std::string& outDir)
{
   boost::filesystem::path out(outDir);
   if (!boost::filesystem::exists(out)) boost::filesystem::create_directories(out);

   return "-Fp\"" + out.string() + "\"/PrecompiledHeader.pch ";
}

inline std::string I (const std::vector<std::string>& includes)
{
   std::string ret;
   std::for_each(includes.begin(), includes.end(), [&ret] (const std::string& s) { ret += "-I\"" + s + "\" "; });
   return ret;
}

inline std::string MT (bool debug, bool crtStatic)
{
   std::string ret;
   if (crtStatic) ret = "-MT";
   else ret = "-MD";

   if (debug) ret += "d";

   return ret + " ";
}

inline std::string O (bool debug)
{
   if (debug) return "-Od ";
   else return "-Ox ";
}

inline std::string RTC (bool debug)
{
   if (debug) return "-RTC1 ";
   else return "";
}

inline std::string GS (bool debug)
{
   if (debug) return "-GS ";
   else return "-GS- ";
}

inline std::string Z (bool debug)
{
   if (debug) return "-Zi ";
   else return "";
}

inline std::string Yu (const std::string& precompiledHeader)
{
   std::string ret;
   if (precompiledHeader.size()) ret += "-Yu\"" + precompiledHeader + "\" ";
   return ret;
}

inline std::string FI (const std::string& precompiledHeader)
{
   std::string ret;
   if (precompiledHeader.size()) ret += "-FI\"" + precompiledHeader + "\" ";
   return ret;
}

inline std::string D (bool debug, const std::vector<std::string>& defines)
{
   std::string ret = "-D_SCL_SECURE_NO_WARNINGS ";

   if (debug) ret += "-D_DEBUG ";
   else ret += "-DNDEBUG ";

   std::for_each(defines.begin(), defines.end(), [&ret] (const std::string& s) { ret += "-D" + s + " "; });

   return ret;
}

inline std::string W (int warningLevel, bool warningAsError, const std::vector<int>& warningDisable)
{
   std::string ret;

   ret = "-W" + boost::lexical_cast<std::string>(warningLevel) + " ";
   if (warningAsError) ret += "-WX ";

   std::for_each(warningDisable.begin(), warningDisable.end(), [&ret] (int d) {
      ret += "-wd" + boost::lexical_cast<std::string>(d) + " ";
   });

   return ret;
}

inline std::string EnvironmentCompiler(bool debug)
{
   std::string ret;

   const char* env = std::getenv("FB_COMPILER");
   if (env) ret += std::string(env) + " ";

   if (debug) {
      env = std::getenv("FB_COMPILER_DEBUG");
      if (env) ret += std::string(env) + " ";
   }
   else {
      env = std::getenv("FB_COMPILER_RELEASE");
      if (env) ret += std::string(env) + " ";
   }

   return ret;
}

void Compiler::Compile ()
{
   CheckParams();
   if (objDir.empty()) throw std::runtime_error("Missing 'Outdir'");

   if (!NeedsRebuild()) return;

   std::cout << "\nCompiling (" << ToolChain::ToolChain() << " " << ToolChain::Platform() << ")"<< std::endl;

   if (beforeCompile) beforeCompile();

   DeleteOutOfDateObjectFiles();
   CompilePrecompiledHeaders();
   CompileFiles();
}

std::string Compiler::CommandLine () const
{
   std::string command = "-nologo -c -EHa -GF -Gm- -FC -DWIN32 -DWINDOWS ";
   if (ToolChain::Platform() == "x86") command += "-arch:SSE2 ";
   command += D(debug, defines);
   command += MP(threads);
   command += MT(debug, crtStatic);
   command += O(debug);
   command += RTC(debug);
   command += GS(debug);
   command += Z(debug);
   command += I(includes);
   command += W(warnLevel, warningAsError, warningDisable);
   command += args + " ";
   command += Fo(objDir);
   command += Fp(objDir);
   command += EnvironmentCompiler(debug);

   return command;
}

void Compiler::CompilePrecompiledHeaders ()
{
   if (outOfDate.empty()) return;

   if (!precompiledCpp.size()) return;

   boost::filesystem::path cpp = boost::filesystem::canonical(precompiledCpp);
   cpp.make_preferred();

   auto it = std::find_if(outOfDate.cbegin(), outOfDate.cend(), [&cpp] (const std::string& f) -> bool {
      return boost::filesystem::equivalent(cpp, f);
   });

   if (it == outOfDate.cend()) return;

   outOfDate.erase(it);

   boost::filesystem::path pch = boost::filesystem::path(objDir) / "PrecompiledHeader.pch";
   if (boost::filesystem::exists(pch)) boost::filesystem::remove(pch);

   std::string command =  "CL " + CommandLine();
   command += "-Yc\"" + boost::filesystem::path(precompiledHeader).filename().string() + "\" ";
   command += cpp.string();

   std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
   int rc = std::system(cmd.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");
}

static bool CanMultiThread (const std::string& file)
{
   size_t size = static_cast<size_t>(boost::filesystem::file_size(file));

   boost::interprocess::file_mapping mapping(file.c_str(), boost::interprocess::read_only);
   boost::interprocess::mapped_region region(mapping, boost::interprocess::read_only, 0, size);

   const char* it = static_cast<const char*>(region.get_address());
   const char* end = it + size;

   static const std::string findMe = "#import";

   if (std::search(it, end, findMe.begin(), findMe.end()) != end) return false;

   return true;
}

void Compiler::CompileFiles ()
{
   if (outOfDate.empty()) return;

   std::vector<std::string> cantMP;

   std::string command =  CommandLine() + FI(precompiledHeader) + Yu(precompiledHeader);

   std::string sources;
   std::for_each(outOfDate.begin(), outOfDate.end(), [&] (const std::string& s) {
      if (CanMultiThread(s)) sources += "\"" + s + "\" ";
      else cantMP.push_back(s);
   });

   if (sources.size()) {
      command += sources;

      if (command.size() > 8000) {
         std::ofstream responseFile(objDir + "/cl.rsp", std::fstream::trunc);
         responseFile << command;
         command = "cl.exe @" + objDir + "/cl.rsp";
      }
      else {
         command.insert(0, "cl.exe ");
      }

      std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
      int rc = std::system(cmd.c_str());
      if (rc != 0) throw std::runtime_error("Compile Error");
   }

   // These are the files that are using #import, which prevents them from compiling them with multiple threads
   // TODO: Do the multithreading myself, by using multiple instances of cl.exe

   for (size_t i = 0; i < cantMP.size(); ++i) {
      command = "cl.exe " + CommandLine() + FI(precompiledHeader) + Yu(precompiledHeader) + "-MP1 ";
      command += "\"" + cantMP[i] + "\" ";

      std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
      int rc = std::system(cmd.c_str());
      if (rc != 0) throw std::runtime_error("Compile Error");
   }
}

bool Compiler::NeedsRebuild ()
{
   CheckParams();

   outOfDate.clear();

   if (!dependencyCheck) {
      std::copy(allFiles.cbegin(), allFiles.cend(), std::back_inserter(outOfDate));
   }
   else {
      ::CppOutOfDate checker;
      checker.OutDir(objDir);
      checker.Threads(threads);
      checker.Files(allFiles);
      checker.Include(includes);
      checker.PrecompiledHeader(precompiledHeader);
      checker.Go();

      outOfDate = checker.OutOfDate();
   }

   return !outOfDate.empty();
}

void Compiler::DeleteOutOfDateObjectFiles ()
{
   const std::vector<std::string> files = CompiledObjFiles();
   std::for_each(files.begin(), files.end(), [] (const std::string& file) {
      boost::filesystem::remove(file);
   });
}

std::vector<std::string> Compiler::ObjFiles () const
{
   boost::filesystem::path outpath(objDir);

   std::vector<std::string> result;

   std::for_each(allFiles.begin(), allFiles.end(), [&] (const std::string& file) {
      boost::filesystem::path f(file);
      auto outfile = outpath / f.filename();
      outfile.replace_extension(".obj");
      result.push_back(outfile.string());
   });

   return result;
}

std::vector<std::string> Compiler::CompiledObjFiles () const
{
   boost::filesystem::path outpath(objDir);

   std::vector<std::string> result;

   std::for_each(outOfDate.begin(), outOfDate.end(), [&] (const std::string& file) {
      boost::filesystem::path f(file);
      auto outfile = outpath / f.filename();
      outfile.replace_extension(".obj");
      result.push_back(outfile.string());
   });

   return result;
}

void Compiler::CheckParams ()
{
   if (objDir.empty()) objDir = debug ? "Debug" : "Release";
}
