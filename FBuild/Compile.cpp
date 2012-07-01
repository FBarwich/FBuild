/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Compile.h"

#include <algorithm>
#include <cstdlib>

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




void Compile::Config (const std::string& v)
{
   if (v.empty()) debug = false;
   else {
      if (v != "Debug" && v != "Release") throw std::runtime_error("'Config' must be 'Release' or 'Debug' (Or empty for default, which is Release)");
      debug = v == "Debug";
   }
}

void Compile::CRT (const std::string& v)
{
   if (v.empty()) crtStatic = false;
   else {
      if (v != "Static" && v != "Dynamic") throw std::runtime_error("'CRT' must be 'Dynamic' or 'Static' (Or empty for default, which is Dynamic)");
      crtStatic = v == "Static";
   }
}

void Compile::Go ()
{
   if (outDir.empty()) throw std::runtime_error("Missing 'Outdir'");

   CompilePrecompiledHeaders();
   CompileFiles();
}

std::string Compile::CommandLine () const
{
   std::string command = "cl.exe -nologo -c -EHa -GF -Gm- -DWIN32 ";
   command += D(debug, defines);
   command += MP(threads);
   command += MT(debug, crtStatic);
   command += O(debug);
   command += Z(debug);
   command += I(includes);
   command += cc;
   command += Fo(outDir);
   command += Fp(outDir);

   return command;
}

void Compile::CompilePrecompiledHeaders ()
{
   if (files.empty()) return;

   if (!precompiledCpp.size()) return;

   boost::filesystem::path cpp(precompiledCpp);

   auto it = std::find_if(files.cbegin(), files.cend(), [&cpp] (const std::string& f) -> bool {
      return boost::filesystem::equivalent(cpp, f);
   });

   if (it == files.cend()) return;

   files.erase(it);

   std::string command =  CommandLine();
   command += "-Yc\"" + precompiledHeader + "\" ";
   command += cpp.string();

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");
}

void Compile::CompileFiles ()
{
   if (files.empty()) return;

   std::string command =  CommandLine() + FI(precompiledHeader) + Yu(precompiledHeader) + Sources(files);

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");
}
