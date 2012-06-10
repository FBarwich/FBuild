/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Script.h"

#include <string>
#include <iostream>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "../lua-5.2.0/src/lua.hpp"




namespace Impl {
   static std::string PopString (lua_State* luaState)
   {
      size_t len;
      const char* ret = lua_tolstring(luaState, -1, &len);
      lua_pop(luaState, 1);

      if (ret) return std::string(ret, len);
      else return std::string();
   }
}




Script::Script (int argc, char** argv)
{
   luaState = luaL_newstate();
   if (!luaState) throw std::runtime_error("Unable to create Lua-VM");
   luaL_openlibs(luaState);

   std::string cmdline;
   cmdline.reserve(1000);

   for (int i = 1; i < argc; ++i) {
      if (cmdline.size()) cmdline += " ";
      cmdline += argv[i];
   }

   ExecuteString(cmdline, "Commandline");
}

Script::~Script ()
{
   if (luaState) lua_close(luaState);
}

void Script::ExecuteBuffer (const char* source, size_t len, const std::string& name)
{
   int rc = luaL_loadbuffer(luaState, source, len, name.c_str());
   if (rc != LUA_OK) {
      std::string msg = "Error compiling script " + name + ":\n";
      if (rc == LUA_ERRSYNTAX) msg += "Syntax error\n";
      else if (rc == LUA_ERRMEM) msg += "Memory error\n";

      msg += Impl::PopString(luaState);

      throw std::runtime_error(msg);
   }

   rc = lua_pcall(luaState, 0, LUA_MULTRET, 0);
   if (rc != LUA_OK) {
      std::string msg = "Error executing script " + name + ":\n";
      if (rc == LUA_ERRRUN) msg += "Runtime error\n";
      else if (rc == LUA_ERRMEM) msg += "Memory error\n";
      else if (rc == LUA_ERRGCMM) msg += "Garbage collector error\n";

      if (lua_gettop(luaState)) msg += Impl::PopString(luaState);

      throw std::runtime_error(msg);
   }            
}

void Script::ExecuteFile (const boost::filesystem::path& f)
{
   boost::filesystem::path file = boost::filesystem::canonical(f);
   file.make_preferred();

   size_t size = static_cast<size_t>(boost::filesystem::file_size(file));

   boost::interprocess::file_mapping mapping(file.string().c_str(), boost::interprocess::read_only);
   boost::interprocess::mapped_region region(mapping, boost::interprocess::read_only, 0, size);

   ExecuteBuffer(static_cast<const char*>(region.get_address()), size, file.string());
}
