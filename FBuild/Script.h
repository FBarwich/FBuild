/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>

#include <boost/filesystem.hpp>


struct lua_State;


class Script {
   lua_State* luaState;

public:
   Script (int argc, char** argv);
   ~Script ();

   void ExecuteBuffer (const char* source, size_t len, const std::string& name);
   void ExecuteString (const std::string& source, const std::string& name) { ExecuteBuffer(source.c_str(), source.size(), name); }
   void ExecuteFile (const boost::filesystem::path& file);
};