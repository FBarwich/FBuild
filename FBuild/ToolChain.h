/*
* Any copyright is dedicated to the Public Domain.
* http://creativecommons.org/publicdomain/zero/1.0/*
*
* Author: Frank Barwich
*/

#pragma once

#include <string>
#include <string_view>


namespace ToolChain {

   void        ToolChain (std::string_view newToolchain);
   std::string ToolChain ();

   void        Platform (std::string_view newPlatform);
   std::string Platform ();

   std::string SetEnvBatchCall ();
}

