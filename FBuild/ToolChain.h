/*
* Any copyright is dedicated to the Public Domain.
* http://creativecommons.org/publicdomain/zero/1.0/*
*
* Author: Frank Barwich
*/

#pragma once

#include <string>

#include <boost/utility/string_ref.hpp>

namespace ToolChain {

   std::string ToolChain();
   std::string Platform();

   void ToolChain(boost::string_ref toolchain);
   void Platform(boost::string_ref platform);
}

