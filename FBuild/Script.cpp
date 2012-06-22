/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Script.h"
#include "CppOutOfDate.h"
#include "Compile.h"

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

   static double PopNumber (lua_State* L)
   {
      double number = lua_tonumber(L, -1);
      lua_pop(L, 1);
      return number;
   }

   static int PopInt (lua_State* L)
   {
      int integer = lua_tointeger(L, -1);
      lua_pop(L, 1);
      return integer;
   }

   static bool PopBool (lua_State* L)
   {
      bool b = lua_toboolean(L, -1) != 0;
      lua_pop(L, 1);
      return b;
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
               lua_pushinteger(L, ++idx);
               lua_pushstring(L, boost::filesystem::canonical(entry.path()).make_preferred().string().c_str());
               lua_settable(L, -3);
            }
         }
      });

      return 1;
   }

   static int CppOutOfDate (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for CppDepends()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for CppDepends()");

      ::CppOutOfDate checker;

      lua_getfield(L, -1, "Outdir");      checker.OutDir(PopString(L));
      lua_getfield(L, -1, "IgnoreCache"); checker.IgnoreCache(PopBool(L));
      lua_getfield(L, -1, "Threads");     checker.Threads(PopInt(L));


      lua_getfield(L, -1, "Includes");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table for 'Includes'");
      int top = lua_gettop(L);
      lua_pushnil(L);
      while (lua_next(L, top) != 0) {
         if (!lua_isstring(L, -1)) luaL_error(L, "Only strings are permitted for 'Includes'");
         checker.AddIncludePath(lua_tostring(L, -1));
         lua_pop(L, 1);
      }
      lua_pop(L, 1);


      lua_getfield(L, -1, "Files");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table for 'Files'");
      top = lua_gettop(L);
      lua_pushnil(L);
      while (lua_next(L, top) != 0) {
         if (!lua_isstring(L, -1)) luaL_error(L, "Only strings are permitted for 'Files'");
         checker.AddFile(lua_tostring(L, -1));
         lua_pop(L, 1);
      }
      lua_pop(L, 1);


      lua_pop(L, 1);

      checker.Go();

      const std::vector<std::string>& outOfDate = checker.OutOfDate();
      int idx = 0;

      lua_newtable(L);

      std::for_each(outOfDate.begin(), outOfDate.end(), [&] (const std::string& s) {
         lua_pushinteger(L, ++idx);
         lua_pushstring(L, s.c_str());
         lua_settable(L, -3);
      });

      return 1;
   }

   static void CompileOptions (lua_State* L, ::Compile& compile)
   {
      lua_getfield(L, -1, "Outdir");  compile.OutDir(PopString(L));
      lua_getfield(L, -1, "Threads"); compile.Threads(PopInt(L));

      lua_getfield(L, -1, "Includes");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table for 'Includes'");
      int top = lua_gettop(L);
      lua_pushnil(L);
      while (lua_next(L, top) != 0) {
         if (!lua_isstring(L, -1)) luaL_error(L, "Only strings are permitted for 'Includes'");
         compile.AddInclude(lua_tostring(L, -1));
         lua_pop(L, 1);
      }
      lua_pop(L, 1);


      lua_getfield(L, -1, "Files");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table for 'Files'");
      top = lua_gettop(L);
      lua_pushnil(L);
      while (lua_next(L, top) != 0) {
         if (!lua_isstring(L, -1)) luaL_error(L, "Only strings are permitted for 'Files'");
         compile.AddFile(lua_tostring(L, -1));
         lua_pop(L, 1);
      }
      lua_pop(L, 1);
   }

   static int Compile (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for Compile()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for Compile()");

      ::Compile compile;
      CompileOptions(L, compile);

      compile.Go();

      const std::vector<std::string>& obj = compile.ObjectFiles();
      int idx = 0;

      lua_newtable(L);

      std::for_each(obj.begin(), obj.end(), [&] (const std::string& s) {
         lua_pushinteger(L, ++idx);
         lua_pushstring(L, s.c_str());
         lua_settable(L, -3);
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

      lua_pushstring(L, "CppOutOfDate");
      lua_pushcfunction(L, &CppOutOfDate);
      lua_settable(L, -3);

      lua_pushstring(L, "Compile");
      lua_pushcfunction(L, &Compile);
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
