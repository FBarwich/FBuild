#include "RC.h"

#include <algorithm>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include <iostream>

void RC::Go () const
{
   if (files.empty()) return;
   if (outdir.empty()) throw std::runtime_error("Mising 'Outdir'");

   std::for_each(files.cbegin(), files.cend(), [&] (const std::string& file) {
      boost::filesystem::path rc(file);
      rc.replace_extension(".res");
      boost::filesystem::path out(this->outdir);
      auto outfile = out / rc.filename();
      
      std::string command = "RC -fo\"" + outfile.string() + "\"" + file;
      
      int r = std::system(command.c_str());
      if (r != 0) throw std::runtime_error("Error compiling resources");
   });
}