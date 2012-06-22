/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Compile.h"

#include <iostream>
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
   if (!boost::filesystem::exists(out)) boost::filesystem::create_directory(out);

   return "-Fo\"" + out.string() + "\" ";
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

void Compile::Go ()
{
   if (files.empty()) return;

   std::string command = "cl.exe -nologo -c -EHa -GF -Gm- -DWIN32 ";
   command += MP(threads);
   command += MT(debug, crtStatic);
   command += O(debug);
   command += Z(debug);
   command += I(includes);
   command += cc;
   command += Fo(outDir);
   command += Sources(files);

   int rc = std::system(command.c_str());
   if (rc != 0) throw std::runtime_error("Compile Error");
}