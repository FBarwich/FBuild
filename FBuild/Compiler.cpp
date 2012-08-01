/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Compiler.h"
#include "CppOutOfDate.h"
#include "JavaScript.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>



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

inline std::string Sources (const std::vector<std::string>& files)
{
   std::string ret;
   std::for_each(files.begin(), files.end(), [&ret] (const std::string& s) { ret += "\"" + s + "\" "; });
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

inline std::string Z (bool debug)
{
   if (debug) return "-Z7 ";
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
   std::string ret;

   if (debug) ret = "-D_DEBUG -D_SCL_SECURE_NO_WARNINGS ";
   else ret = "-DNDEBUG ";

   std::for_each(defines.begin(), defines.end(), [&ret] (const std::string& s) { ret += "-D" + s + " "; });

   return ret;
}

void Compiler::Compile ()
{
   CheckParams();
   if (objDir.empty()) throw std::runtime_error("Missing 'Outdir'");

   if (!NeedsRebuild()) return;

   CompilePrecompiledHeaders();
   CompileFiles();
}

std::string Compiler::CommandLine () const
{
   std::string command = "-nologo -c -EHa -GF -Gm- -DWIN32 -DWINDOWS ";
   command += D(debug, defines);
   command += MP(threads);
   command += MT(debug, crtStatic);
   command += O(debug);
   command += Z(debug);
   command += I(includes);
   command += args + " ";
   command += Fo(objDir);
   command += Fp(objDir);

   return command;
}

void Compiler::CompilePrecompiledHeaders ()
{
   if (outOfDate.empty()) return;

   if (!precompiledCpp.size()) return;

   boost::filesystem::path cpp(precompiledCpp);

   auto it = std::find_if(outOfDate.cbegin(), outOfDate.cend(), [&cpp] (const std::string& f) -> bool {
      return boost::filesystem::equivalent(cpp, f);
   });

   if (it == outOfDate.cend()) return;

   outOfDate.erase(it);

   std::string command =  "CL " + CommandLine();
   command += "-Yc\"" + precompiledHeader + "\" ";
   command += cpp.string();

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");
}

void Compiler::CompileFiles ()
{
   if (outOfDate.empty()) return;

   std::string command =  CommandLine() + FI(precompiledHeader) + Yu(precompiledHeader) + Sources(outOfDate);

   if (command.size() > 8000) {
      std::ofstream responseFile(objDir + "/cl.rsp", std::fstream::trunc);
      responseFile << command;
      command = "cl.exe @" + objDir + "/cl.rsp";
   }
   else {
      command.insert(0, "cl.exe ");
   }

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");
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
      checker.Go();

      outOfDate = checker.OutOfDate();
   }

   return !outOfDate.empty();
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
