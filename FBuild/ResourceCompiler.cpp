/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "ResourceCompiler.h"
#include "CppDepends.h"
#include "ToolChain.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>


inline std::string Inc(const std::vector<std::string>& includes)
{
   std::string ret;
   
   for (const auto& include : includes) {
      ret += "-I\"" + include + "\" ";
   }

   return ret;
}

inline uint64_t LastWriteTime (const std::filesystem::path& file)
{
   return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(file).time_since_epoch()).count();
}


std::string ResourceCompiler::Outfile (const std::string& infile) const
{
   std::filesystem::path rc(infile);
   rc.replace_extension(".res");
   std::filesystem::path out(outdir);
   auto outfile = out / rc.filename();
   return outfile.string();
}

bool ResourceCompiler::NeedsRebuild (const std::string& infile, const std::string& outfile) const
{
   if (!dependencyCheck) return true;
   if (!std::filesystem::exists(outfile)) return true;

   CppDepends dep(infile);
   return LastWriteTime(outfile) < dep.MaxTime(); 
}

void ResourceCompiler::Compile () const
{
   if (files.empty()) return;
   if (outdir.empty()) throw std::runtime_error("Mising 'Outdir'");

   if (!std::filesystem::exists(outdir)) std::filesystem::create_directories(outdir);

   size_t count = 0;

   std::for_each(files.cbegin(), files.cend(), [&] (const std::string& file) {
      std::string outfile = Outfile(file);
      if (NeedsRebuild(file, outfile)) {
         if (++count) std::cout << "\nCompiling Resources (" << ToolChain::ToolChain() << " " << ToolChain::Platform() << ")" << std::endl;
         if (std::filesystem::exists(outfile)) std::filesystem::remove(outfile);
         std::string command = "RC -nologo " + Inc(includes) + " -fo\"" + outfile + "\" " + file;

         std::string cmd = ToolChain::SetEnvBatchCall() + " & " + command;
         int rc = std::system(cmd.c_str());
         if (rc != 0) throw std::runtime_error("Error compiling resources");
      }
   });
}

std::vector<std::string> ResourceCompiler::Outfiles () const
{
   std::vector<std::string> result;

   std::for_each(files.begin(), files.end(), [&] (const std::string& file) {
      result.push_back(Outfile(file));
   });

   return result;
}

