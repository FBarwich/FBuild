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

#include <Shlwapi.h>



namespace Impl {
   // Helper functions for Lua-API
   static std::string PopString (lua_State* L)
   {
      size_t len;
      const char* ret = lua_tolstring(L, -1, &len);
      lua_pop(L, 1);

      if (ret) return std::string(ret, len);
      else return std::string();
   }

   // Functions that are callable from lua code

   static int Glob (lua_State* L)
   {
      std::string path = ".";
      std::string pattern = "*";

      if (lua_gettop(L) == 2) {
         pattern = PopString(L);
         path = PopString(L);
      }
      else if (lua_gettop(L) == 1) {
         pattern = PopString(L);
      }
      else {
         luaL_error(L, "Expected one or two arguments for FBuild.Glob()");
      }

      int idx = 0;
      lua_newtable(L);

      std::for_each(boost::filesystem::directory_iterator(path), boost::filesystem::directory_iterator(), [&] (const boost::filesystem::directory_entry& entry) {
         if (boost::filesystem::is_regular_file(entry.path())) {
            if (PathMatchSpec(entry.path().filename().string().c_str(), pattern.c_str())) {
               lua_pushnumber(L, idx++);
               lua_pushstring(L, boost::filesystem::canonical(entry.path()).make_preferred().string().c_str());
               lua_settable(L, -3);
            }
         }
      });

      return 1;
   }

   // Register the avove Lua-Callable functions. All Functions are in the table "FBuild".
   static void RegisterMyFuncs (lua_State* L)
   {
      lua_newtable(L);

      lua_pushstring(L, "Glob");
      lua_pushcfunction(L, &Glob);
      lua_settable(L, -3);

      lua_setglobal(L, "FBuild");
   }
}




Script::Script (int argc, char** argv)
{
   luaState = luaL_newstate();
   if (!luaState) throw std::runtime_error("Unable to create Lua-VM");
   luaL_openlibs(luaState);
   Impl::RegisterMyFuncs(luaState);

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
