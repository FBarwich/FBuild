#pragma once

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

class Copy {
   std::string source;
   std::string dest;
   bool        ignoreTimestamp;
   int         copied;

   struct CopyMe {
      boost::filesystem::path source;
      boost::filesystem::path dest;
   };

   std::vector<CopyMe> copyMe;

   void DoFile (boost::filesystem::path source, boost::filesystem::path dest);

public:
   Copy () : ignoreTimestamp(false), copied(0) { }

   void Source (std::string source)         { this->source = std::move(source); }
   void Dest (std::string dest)             { this->dest = std::move(dest); }
   void IgnoreTimestamp (bool ign)          { ignoreTimestamp = ign; }

   int Go ();
};

