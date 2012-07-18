/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Script.h"
#include "CppOutOfDate.h"
#include "Compile.h"
#include "Lib.h"
#include "FileOutOfDate.h"
#include "Link.h"
#include "Copy.h"
#include "FileToCpp.h"
#include "RC.h"

#include <string>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>

#include "../lua-5.2.0/src/lua.hpp"

#include <Shlwapi.h>





namespace Impl {
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





   static void ExecuteBuffer (lua_State* L, const char* source, size_t len, const std::string& name)
   {
      int rc = luaL_loadbuffer(L, source, len, name.c_str());
      if (rc != LUA_OK) {
         std::string msg = "Error compiling script " + name + ":\n";
         if (rc == LUA_ERRSYNTAX) msg += "Syntax error\n";
         else if (rc == LUA_ERRMEM) msg += "Memory error\n";

         msg += Impl::PopString(L);

         throw std::runtime_error(msg);
      }

      rc = lua_pcall(L, 0, LUA_MULTRET, 0);
      if (rc != LUA_OK) {
         std::string msg = "Error executing script " + name + ":\n";
         if (rc == LUA_ERRRUN) msg += "Runtime error\n";
         else if (rc == LUA_ERRMEM) msg += "Memory error\n";
         else if (rc == LUA_ERRGCMM) msg += "Garbage collector error\n";

         if (lua_gettop(L)) msg += Impl::PopString(L);

         throw std::runtime_error(msg);
      }            
   }

   static void ExecuteFile (lua_State* L, const boost::filesystem::path& f)
   {
      boost::filesystem::path file = boost::filesystem::canonical(f);
      file.make_preferred();

      size_t size = static_cast<size_t>(boost::filesystem::file_size(file));

      boost::interprocess::file_mapping mapping(file.string().c_str(), boost::interprocess::read_only);
      boost::interprocess::mapped_region region(mapping, boost::interprocess::read_only, 0, size);

      ExecuteBuffer(L, static_cast<const char*>(region.get_address()), size, file.string());
   }





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

      if (boost::filesystem::exists(path)) {
         std::for_each(boost::filesystem::directory_iterator(path), boost::filesystem::directory_iterator(), [&] (const boost::filesystem::directory_entry& entry) {
            if (boost::filesystem::is_regular_file(entry.path())) {
               if (PathMatchSpec(entry.path().filename().string().c_str(), pattern.c_str())) {
                  lua_pushinteger(L, ++idx);
                  lua_pushstring(L, boost::filesystem::canonical(entry.path()).make_preferred().string().c_str());
                  lua_settable(L, -3);
               }
            }
         });
      }

      return 1;
   }

   inline std::vector<std::string> StringArray (lua_State* L, const std::string& name)
   {
      std::vector<std::string> ret;

      lua_getfield(L, -1, name.c_str());
      if (!lua_isnil(L, -1)) {
         if (!lua_istable(L, -1)) luaL_error(L, "Expected array for '%s'", name.c_str());
         int top = lua_gettop(L);
         lua_pushnil(L);
         while (lua_next(L, top) != 0) {
            if (!lua_isstring(L, -1)) luaL_error(L, "Only strings are permitted for '%s'", name.c_str());
            ret.push_back(lua_tostring(L, -1));
            lua_pop(L, 1);
         }
      }
      lua_pop(L, 1);

      return ret;
   }

   inline std::string String (lua_State* L, const std::string& name)
   {
      lua_getfield(L, -1, name.c_str());
      return PopString(L);
   }

   inline int Int (lua_State* L, const std::string& name)
   {
      lua_getfield(L, -1, name.c_str());
      return PopInt(L);
   }

   inline bool Bool (lua_State* L, const std::string& name)
   {
      lua_getfield(L, -1, name.c_str());
      return PopBool(L);
   }

   inline bool Bool (lua_State* L, const std::string& name, bool def)
   {
      lua_getfield(L, -1, name.c_str());
      if (lua_isnil(L, -1)) {
         lua_pop(L, 1);
         return def;
      }
      return PopBool(L);
   }

   static int CppOutOfDate (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for CppOutOfDate()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for CppOutOfDate()");

      ::CppOutOfDate checker;

      checker.OutDir(String(L, "Outdir"));
      checker.IgnoreCache(Bool(L, "IgnoreCache"));
      checker.Threads(Int(L, "Threads"));
      checker.Files(StringArray(L, "Files"));
      checker.Include(StringArray(L, "Includes"));

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

   static int Compile (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for Compile()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for Compile()");

      ::Compile compile;
      compile.Config(String(L, "Config"));
      compile.CRT(String(L, "CRT"));
      compile.CC(String(L, "CC"));
      compile.OutDir(String(L, "Outdir"));
      compile.Threads(Int(L, "Threads"));
      compile.Include(StringArray(L, "Includes"));
      compile.Define(StringArray(L, "Defines"));
      compile.Files(StringArray(L, "Files"));

      lua_getfield(L, -1, "PrecompiledHeader");
      if (!lua_isnil(L, -1)) {
         if (!lua_istable(L, -1)) luaL_error(L, "Expected table for 'PrecompiledHeader'");

         lua_getfield(L, -1, "Header");
         compile.PrecompiledHeader(PopString(L));

         lua_getfield(L, -1, "Cpp");
         compile.PrecompiledCpp(PopString(L));
      }
      lua_pop(L, 1);

      compile.Go();

      return 0;
   }

   static int Lib (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for Lib()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for Lib()");

      ::Lib lib;
      lib.Output(String(L, "Output"));
      lib.Files(StringArray(L, "Files"));
      lib.Go();

      return 0;
   }

   static int FileOutOfDate (lua_State* L)
   {
      if (lua_gettop(L) != 2) luaL_error(L, "Expected two arguments for FileOutOfDate()");

      ::FileOutOfDate outOfDate;

      if (lua_istable(L, -1)) {
         int top = lua_gettop(L);
         lua_pushnil(L);
         while (lua_next(L, top) != 0) {
            if (!lua_isstring(L, -1)) luaL_error(L, "Only strings are permitted for 'FileOutOfDate'");
            outOfDate.AddFile(lua_tostring(L, -1));
            lua_pop(L, 1);
         }
         lua_pop(L, 1);
      }
      else {
         outOfDate.AddFile(PopString(L));
      }

      outOfDate.Parent(PopString(L));

      lua_pushboolean(L, outOfDate.Go());
      return 1;
   }

   static int BuildStaticLib (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for BuildStaticLib()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for BuildStaticLib()");

      ::CppOutOfDate checker;
      checker.OutDir(String(L, "Outdir"));
      checker.IgnoreCache(Bool(L, "IgnoreCache"));
      checker.Threads(Int(L, "Threads"));
      checker.Files(StringArray(L, "Files"));
      checker.Include(StringArray(L, "Includes"));
      checker.Go();

      std::vector<std::string> outOfDate = checker.OutOfDate();

      if (!outOfDate.empty()) {
         ::Compile compile;
         compile.Config(String(L, "Config"));
         compile.CRT(String(L, "CRT"));
         compile.CC(String(L, "CC"));
         compile.OutDir(String(L, "Outdir"));
         compile.Threads(Int(L, "Threads"));
         compile.Include(StringArray(L, "Includes"));
         compile.Define(StringArray(L, "Defines"));
         compile.Files(std::move(outOfDate));

         lua_getfield(L, -1, "PrecompiledHeader");
         if (!lua_isnil(L, -1)) {
            if (!lua_istable(L, -1)) luaL_error(L, "Expected table for 'PrecompiledHeader'");

            lua_getfield(L, -1, "Header");
            compile.PrecompiledHeader(PopString(L));

            lua_getfield(L, -1, "Cpp");
            compile.PrecompiledCpp(PopString(L));
         }
         lua_pop(L, 1);

         compile.Go();
      }

      ::Lib lib;
      lib.Output(String(L, "Output"));
      lib.AutoFilesFromCpp(String(L, "Outdir"), StringArray(L, "Files"));
      lib.Go();


      return 0;
   }

   static int Link (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for Link()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for Link()");

      ::Link link;
      link.Config(String(L, "Config"));
      link.Output(String(L, "Output"));
      link.ImportLib(String(L, "ImportLib"));
      link.Def(String(L, "Def"));
      link.Libpath(StringArray(L, "Libpath"));
      link.Libs(StringArray(L, "Libs"));
      link.Files(StringArray(L, "Files"));
      link.Go();

      return 0;
   }

   static int BuildExe (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for BuildExe()/BuildDynamicLib()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for BuildExe()/BuildDynamicLib()");

      ::CppOutOfDate checker;
      checker.OutDir(String(L, "Outdir"));
      checker.IgnoreCache(Bool(L, "IgnoreCache"));
      checker.Threads(Int(L, "Threads"));
      checker.Files(StringArray(L, "Files"));
      checker.Include(StringArray(L, "Includes"));
      checker.Go();

      std::vector<std::string> outOfDate = checker.OutOfDate();

      if (!outOfDate.empty()) {
         ::Compile compile;
         compile.Config(String(L, "Config"));
         compile.CRT(String(L, "CRT"));
         compile.CC(String(L, "CC"));
         compile.OutDir(String(L, "Outdir"));
         compile.Threads(Int(L, "Threads"));
         compile.Include(StringArray(L, "Includes"));
         compile.Define(StringArray(L, "Defines"));
         compile.Files(std::move(outOfDate));

         lua_getfield(L, -1, "PrecompiledHeader");
         if (!lua_isnil(L, -1)) {
            if (!lua_istable(L, -1)) luaL_error(L, "Expected table for 'PrecompiledHeader'");

            lua_getfield(L, -1, "Header");
            compile.PrecompiledHeader(PopString(L));

            lua_getfield(L, -1, "Cpp");
            compile.PrecompiledCpp(PopString(L));
         }
         lua_pop(L, 1);

         compile.Go();
      }

      ::Link link;
      link.Config(String(L, "Config"));
      link.Output(String(L, "Output"));
      link.ImportLib(String(L, "ImportLib"));
      link.Def(String(L, "Def"));
      link.Libpath(StringArray(L, "Libpath"));
      link.Libs(StringArray(L, "Libs"));
      link.AutoFilesFromCpp(String(L, "Outdir"), StringArray(L, "Files"));
      link.Go();

      return 0;
   }

   static int Build (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for Build()");

      auto current = boost::filesystem::current_path();
      boost::filesystem::current_path(PopString(L));

      ExecuteFile(L, "FBuild.lua");

      boost::filesystem::current_path(current);

      return 0;
   }

   static int System (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for System()");

      std::string command = PopString(L);
      int rc = std::system(command.c_str());
      lua_pushinteger(L, rc);

      return 1;
   }

   static int Copy (lua_State* L)
   {
      ::Copy copy;

      std::string source;
      std::string dest;
      bool ignoreTimestamp = false;

      if (lua_gettop(L) == 3) copy.IgnoreTimestamp(PopBool(L));
      if (lua_gettop(L) != 2) luaL_error(L, "Expected two or three arguments for Copy()");
      copy.Dest(PopString(L));
      copy.Source(PopString(L));

      int rc = copy.Go();

      lua_pushinteger(L, rc);
      return 1;
   }

   static int ChangeDirectory (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for ChangeDirectory()");

      boost::filesystem::current_path(PopString(L));

      return 0;
   }

   static int FileToCpp (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for FileToCpp()");

      ::FileToCpp file2cpp;
      file2cpp.CheckDependency(Bool(L, "CheckDependency", true));
      file2cpp.Infile (String(L, "InFile"));
      file2cpp.Outfile (String(L, "OutFile"));
      file2cpp.Namespace (String(L, "Namespace"));
      file2cpp.Arry (String(L, "VarNameArry"));
      file2cpp.Ptr (String(L, "VarNamePointer"));
      file2cpp.Intro (String(L, "Intro"));
      file2cpp.Outro (String(L, "Outro"));
      file2cpp.Additional (String(L, "Additional"));
      file2cpp.Const(Bool(L, "Const", true));

      file2cpp.Go();

      return 0;
   }

   static int RC (lua_State* L)
   {
      if (lua_gettop(L) != 1) luaL_error(L, "Expected one argument for RC()");
      if (!lua_istable(L, -1)) luaL_error(L, "Expected table as argument for RC()");

      ::RC rc;
      rc.Outdir(String(L, "Outdir"));
      rc.Files(StringArray(L, "Res"));
      rc.Go();

      return 0;
   }


   // Register the avove Lua-Callable functions. All Functions are in the table "FBuild".

   inline void RegisterFunc (lua_State* L, const char* funcname, int (*func) (lua_State* L))
   {
      lua_pushstring(L, funcname);
      lua_pushcfunction(L, func);
      lua_settable(L, -3);
   }

   static void RegisterMyFuncs (lua_State* L)
   {
      lua_newtable(L);

      RegisterFunc(L, "Glob", &Glob);
      RegisterFunc(L, "CppOutOfDate", &CppOutOfDate);
      RegisterFunc(L, "Compile", &Compile);
      RegisterFunc(L, "Lib", &Lib);
      RegisterFunc(L, "FileOutOfDate", &FileOutOfDate);
      RegisterFunc(L, "BuildStaticLib", &BuildStaticLib);
      RegisterFunc(L, "Link", &Link);
      RegisterFunc(L, "BuildExe", &BuildExe);
      RegisterFunc(L, "BuildDynamicLib", &BuildExe);
      RegisterFunc(L, "Build", &Build);
      RegisterFunc(L, "System", &System);
      RegisterFunc(L, "Copy", &Copy);
      RegisterFunc(L, "ChangeDirectory", &ChangeDirectory);
      RegisterFunc(L, "FileToCpp", &FileToCpp);
      RegisterFunc(L, "RC", &RC);

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
   Impl::ExecuteBuffer(luaState, source, len, name);
}

void Script::ExecuteFile (const boost::filesystem::path& f)
{
   Impl::ExecuteFile(luaState, f);
}
