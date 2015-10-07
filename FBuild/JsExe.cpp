/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsExe.h"

#include <boost/filesystem.hpp>


duk_ret_t JsExe::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Exe/Dll constructor expected");

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      duk_push_pointer(duktapeContext, new JsExe);
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsExe::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsExe::Build, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Build");

      duk_push_c_function(duktapeContext, JsExe::Files, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Files");

      duk_push_c_function(duktapeContext, JsExe::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsExe::CRT, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "CRT");

      duk_push_c_function(duktapeContext, JsExe::ObjDir, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "ObjDir");

      duk_push_c_function(duktapeContext, JsExe::Includes, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Includes");

      duk_push_c_function(duktapeContext, JsExe::Defines, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Defines");

      duk_push_c_function(duktapeContext, JsExe::Threads, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Threads");

      duk_push_c_function(duktapeContext, JsExe::CompileArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "CompileArgs");

      duk_push_c_function(duktapeContext, JsExe::CompileArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Args");

      duk_push_c_function(duktapeContext, JsExe::PrecompiledHeader, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "PrecompiledHeader");

      duk_push_c_function(duktapeContext, JsExe::WarningLevel, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningLevel");

      duk_push_c_function(duktapeContext, JsExe::WarningAsError, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningAsError");

      duk_push_c_function(duktapeContext, JsExe::WarningDisable, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningDisable");

      duk_push_c_function(duktapeContext, JsExe::Output, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Output");

      duk_push_c_function(duktapeContext, JsExe::LibPath, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "LibPath");

      duk_push_c_function(duktapeContext, JsExe::Libs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Libs");

      duk_push_c_function(duktapeContext, JsExe::ImportLib, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "ImportLib");

      duk_push_c_function(duktapeContext, JsExe::Def, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Def");

      duk_push_c_function(duktapeContext, JsExe::LinkArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "LinkArgs");

      duk_push_c_function(duktapeContext, JsExe::Res, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Res");

      duk_push_c_function(duktapeContext, JsExe::BeforeCompile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "BeforeCompile");

      duk_push_c_function(duktapeContext, JsExe::BeforeLink, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "BeforeLink");

      duk_push_c_function(duktapeContext, JsExe::Create, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Create");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsExe>(duktapeContext);
   return 0;
}

duk_ret_t JsExe::Build(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.Build().c_str());
      else if (args == 1) obj->compiler.Build(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::Build() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Files(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Files());
      else obj->compiler.Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->compiler.DependencyCheck());
      else if (args == 1) obj->compiler.DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::CRT(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.CRT().c_str());
      else if (args == 1) obj->compiler.CRT(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::CRT() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::ObjDir(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.ObjDir().c_str());
      else if (args == 1) obj->compiler.ObjDir(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::ObjDir() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Includes(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Includes());
      else {
         auto includes = JavaScriptHelper::AsStringVector(duktapeContext, args);
         obj->compiler.Includes(includes);
         obj->resourceCompiler.Includes(includes);
      }

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Defines(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Defines());
      else obj->compiler.Defines(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Threads(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_int(duktapeContext, obj->compiler.Threads());
      else if (args == 1) obj->compiler.Threads(duk_require_int(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::Threads() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::CompileArgs(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.Args().c_str());
      else obj->compiler.Args(JavaScriptHelper::AsString(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::PrecompiledHeader(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.PrecompiledHeader().c_str());
      else if (args == 2) obj->compiler.PrecompiledHeader(duk_require_string(duktapeContext, 0), duk_require_string(duktapeContext, 1));
      else JavaScriptHelper::Throw(duktapeContext, "Two arguments for Exe::PrecompiledHeader() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::WarningLevel(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_int(duktapeContext, obj->compiler.WarnLevel());
      else if (args == 1) obj->compiler.WarnLevel(duk_require_int(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::WarningLevel() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::WarningAsError(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->compiler.WarningAsError());
      else if (args == 1) obj->compiler.WarningAsError(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::WarningAsError() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::WarningDisable(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.WarningDisable());
      else obj->compiler.WarningDisable(JavaScriptHelper::AsIntVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Output(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.Output().c_str());
      else if (args == 1) obj->linker.Output(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::Output() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::LibPath(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->linker.Libpath());
      else obj->linker.Libpath(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Libs(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->linker.Libs());
      else obj->linker.Libs(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::ImportLib(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.ImportLib().c_str());
      else if (args == 1) obj->linker.ImportLib(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::ImportLib() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Def(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.Def().c_str());
      else if (args == 1) obj->linker.Def(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::Def() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::LinkArgs(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.Args().c_str());
      else obj->linker.Args(JavaScriptHelper::AsString(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Res(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->resourceCompiler.Files());
      else obj->resourceCompiler.Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::BeforeCompile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) {
         JavaScriptHelper::PushStashedCallback(duktapeContext, "JsExe::BeforeCompile", obj);
      }
      else if (args == 1) {
         JavaScriptHelper::StashCallback(duktapeContext, 0, "JsExe::BeforeCompile", obj);
         obj->compiler.BeforeCompile([=] () { JavaScriptHelper::CallStashedCallback(duktapeContext, "JsExe::BeforeCompile", obj); });
      }
      else {
         JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::BeforeCompile() expected");
      }

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::BeforeLink(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (!args) {
         JavaScriptHelper::PushStashedCallback(duktapeContext, "JsExe::BeforeLink", obj);
      }
      else if (args == 1) {
         JavaScriptHelper::StashCallback(duktapeContext, 0, "JsExe::BeforeLink", obj);
         obj->linker.BeforeLink([=] () { JavaScriptHelper::CallStashedCallback(duktapeContext, "JsExe::BeforeLink", obj); });
      }
      else {
         JavaScriptHelper::Throw(duktapeContext, "One argument for Exe::BeforeLink() expected");
      }

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsExe::Create(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsExe>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Exe::Create() expected");

      obj->compiler.Compile();

      bool rc = !obj->resourceCompiler.Outfiles().empty();
      if (rc) {
         obj->resourceCompiler.DependencyCheck(obj->compiler.DependencyCheck());
         obj->resourceCompiler.Outdir(obj->compiler.ObjDir());
         obj->resourceCompiler.Compile();
      }

      obj->linker.Build(obj->compiler.Build());

      std::vector<std::string> objFiles;
      for (auto&& f : obj->compiler.ObjFiles()) {
         if (boost::filesystem::file_size(f) != 0) objFiles.push_back(f);
      }
      obj->linker.Files(objFiles);

      if (rc) obj->linker.AddFiles(obj->resourceCompiler.Outfiles());
      obj->linker.DependencyCheck(obj->compiler.DependencyCheck());
      obj->linker.Link();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

void JsExe::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsExe::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Exe");

   duk_push_c_function(duktapeContext, JsExe::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Dll");

   duk_pop(duktapeContext);
}

