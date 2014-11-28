/*
* Any copyright is dedicated to the Public Domain.
* http://creativecommons.org/publicdomain/zero/1.0/*
*
* Author: Frank Barwich
*/

#include "ToolChain.h"

#include <mutex>


namespace ToolChain {

   static std::mutex mutex;
   static std::string toolchain;
   static std::string platform = "x86";

   std::string ToolChain()
   {
      if (toolchain.size()) return toolchain;
      // TODO: Get most recent VC-Intallation, or use the environment alrady in use, if any.
      return "default";
   }

   std::string Platform()
   {
      if (platform.size()) return platform;
      // TODO: Check if already on an environment, if so, use that
      return "x86";
   }

   void ToolChain(boost::string_ref toolchain)
   {
      // TODO: Check if valid and installed;
      ToolChain::toolchain.assign(toolchain.begin(), toolchain.end());
   }

   void Platform(boost::string_ref platform)
   {
      if (platform != "x86" && platform != "x64") throw std::runtime_error("Only x86 and x64 platforms are supported");
      ToolChain::platform.assign(platform.begin(), platform.end());
   }


}
