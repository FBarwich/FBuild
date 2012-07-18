#pragma once

#include <string>
#include <vector>

class RC {
   std::string outdir;
   std::vector<std::string> files;
public:

   void Outdir (const std::string& v)        { outdir = v; }
   void Files (std::vector<std::string>&& v) { files = std::move(v); }

   void Go () const;
};

