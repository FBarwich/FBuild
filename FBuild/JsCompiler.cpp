/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsCompiler.h"


duk_ret_t JsCompiler::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Lib constructor expected");

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      duk_push_pointer(duktapeContext, new JsCompiler);
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsCompiler::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsCompiler::Build, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Build");

      duk_push_c_function(duktapeContext, JsCompiler::Files, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Files");

      duk_push_c_function(duktapeContext, JsCompiler::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsCompiler::CRT, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "CRT");

      duk_push_c_function(duktapeContext, JsCompiler::ObjDir, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "ObjDir");

      duk_push_c_function(duktapeContext, JsCompiler::Includes, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Includes");

      duk_push_c_function(duktapeContext, JsCompiler::Defines, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Defines");

      duk_push_c_function(duktapeContext, JsCompiler::Threads, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Threads");

      duk_push_c_function(duktapeContext, JsCompiler::CompileArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "CompileArgs");

      duk_push_c_function(duktapeContext, JsCompiler::CompileArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Args");

      duk_push_c_function(duktapeContext, JsCompiler::PrecompiledHeader, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "PrecompiledHeader");

      duk_push_c_function(duktapeContext, JsCompiler::WarningLevel, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningLevel");

      duk_push_c_function(duktapeContext, JsCompiler::WarningAsError, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningAsError");

      duk_push_c_function(duktapeContext, JsCompiler::WarningDisable, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningDisable");

      duk_push_c_function(duktapeContext, JsCompiler::ObjFiles, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "ObjFiles");

      duk_push_c_function(duktapeContext, JsCompiler::CompiledObjFiles, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "CompiledObjFiles");

      duk_push_c_function(duktapeContext, JsCompiler::BeforeCompile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "BeforeCompile");

      duk_push_c_function(duktapeContext, JsCompiler::Compile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Compile");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);
   return 0;
}

duk_ret_t JsCompiler::Build(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.Build().c_str());
      else if (args == 1) obj->compiler.Build(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Compiler::Build() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::Files(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Files());
      else obj->compiler.Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->compiler.DependencyCheck());
      else if (args == 1) obj->compiler.DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Compiler::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::CRT(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.CRT().c_str());
      else if (args == 1) obj->compiler.CRT(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Compiler::CRT() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::ObjDir(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.ObjDir().c_str());
      else if (args == 1) obj->compiler.ObjDir(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Compiler::ObjDir() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::Includes(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Includes());
      else obj->compiler.Includes(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::Defines(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Defines());
      else obj->compiler.Defines(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::Threads(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_int(duktapeContext, obj->compiler.Threads());
      else if (args == 1) obj->compiler.Threads(duk_require_int(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Compiler::Threads() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::CompileArgs(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.Args().c_str());
      else obj->compiler.Args(JavaScriptHelper::AsString(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::PrecompiledHeader(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.PrecompiledHeader().c_str());
      else if (args == 2) obj->compiler.PrecompiledHeader(duk_require_string(duktapeContext, 0), duk_require_string(duktapeContext, 1));
      else JavaScriptHelper::Throw(duktapeContext, "Two arguments for Compiler::PrecompiledHeader() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::WarningLevel(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_int(duktapeContext, obj->compiler.WarnLevel());
      else if (args == 1) obj->compiler.WarnLevel(duk_require_int(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Compiler::WarningLevel() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::WarningAsError(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->compiler.WarningAsError());
      else if (args == 1) obj->compiler.WarningAsError(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Compiler::WarningAsError() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::WarningDisable(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.WarningDisable());
      else obj->compiler.WarningDisable(JavaScriptHelper::AsIntVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::ObjFiles(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.ObjFiles());
      else JavaScriptHelper::Throw(duktapeContext, "No arguments for Compiler::ObjFiles() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::CompiledObjFiles(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.CompiledObjFiles());
      else JavaScriptHelper::Throw(duktapeContext, "No arguments for Compiler::CompiledObjFiles() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::BeforeCompile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (!args) {
         JavaScriptHelper::PushStashedCallback(duktapeContext, "JsCompiler::BeforeCompile", obj);
      }
      else if (args == 1) {
         JavaScriptHelper::StashCallback(duktapeContext, 0, "JsCompiler::BeforeCompile", obj);
         obj->compiler.BeforeCompile([=] () { JavaScriptHelper::CallStashedCallback(duktapeContext, "JsCompiler::BeforeCompile", obj); });
      }
      else {
         JavaScriptHelper::Throw(duktapeContext, "One argument for Compiler::BeforeCompile() expected");
      }

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsCompiler::Compile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsCompiler>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Compiler::Compile() expected");

      obj->compiler.Compile();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

void JsCompiler::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsCompiler::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Compiler");

   duk_pop(duktapeContext);
}

