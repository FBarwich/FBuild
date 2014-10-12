/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsLib.h"

#include <iostream>

#include <boost/lexical_cast.hpp>


duk_ret_t JsLib::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Lib constructor expected");

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      JsLib* lib = new JsLib;

      duk_push_pointer(duktapeContext, lib);
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsLib::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsLib::Build, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Build");

      duk_push_c_function(duktapeContext, JsLib::Files, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Files");

      duk_push_c_function(duktapeContext, JsLib::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsLib::Output, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Output");

      duk_push_c_function(duktapeContext, JsLib::CRT, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "CRT");

      duk_push_c_function(duktapeContext, JsLib::ObjDir, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "ObjDir");

      duk_push_c_function(duktapeContext, JsLib::Includes, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Includes");

      duk_push_c_function(duktapeContext, JsLib::Defines, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Defines");

      duk_push_c_function(duktapeContext, JsLib::Threads, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Threads");

      duk_push_c_function(duktapeContext, JsLib::CompileArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "CompileArgs");

      duk_push_c_function(duktapeContext, JsLib::CompileArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Args");

      duk_push_c_function(duktapeContext, JsLib::PrecompiledHeader, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "PrecompiledHeader");

      duk_push_c_function(duktapeContext, JsLib::WarningLevel, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningLevel");

      duk_push_c_function(duktapeContext, JsLib::WarningAsError, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningAsError");

      duk_push_c_function(duktapeContext, JsLib::WarningDisable, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "WarningDisable");

      duk_push_c_function(duktapeContext, JsLib::BeforeCompile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "BeforeCompile");

      duk_push_c_function(duktapeContext, JsLib::BeforeLink, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "BeforeLink");

      duk_push_c_function(duktapeContext, JsLib::Create, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Create");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsLib>(duktapeContext);
   return 0;
}

duk_ret_t JsLib::Build(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.Build().c_str());
      else if (args == 1) obj->compiler.Build(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::Build() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::Files(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Files());
      else obj->compiler.Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->compiler.DependencyCheck());
      else if (args == 1) obj->compiler.DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::Output(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->librarian.Output().c_str());
      else if (args == 1) obj->librarian.Output(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::Output() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::CRT(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.CRT().c_str());
      else if (args == 1) obj->compiler.CRT(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::CRT() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::ObjDir(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.ObjDir().c_str());
      else if (args == 1) obj->compiler.ObjDir(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::ObjDir() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::Includes(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Includes());
      else obj->compiler.Includes(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::Defines(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.Defines());
      else obj->compiler.Defines(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::Threads(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_int(duktapeContext, obj->compiler.Threads());
      else if (args == 1) obj->compiler.Threads(duk_require_int(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::Threads() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::CompileArgs(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.Args().c_str());
      else obj->compiler.Args(JavaScriptHelper::AsString(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::PrecompiledHeader(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->compiler.PrecompiledHeader().c_str());
      else if (args == 2) obj->compiler.PrecompiledHeader(duk_require_string(duktapeContext, 0), duk_require_string(duktapeContext, 1));
      else JavaScriptHelper::Throw(duktapeContext, "Two arguments for Lib::PrecompiledHeader() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::WarningLevel(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_int(duktapeContext, obj->compiler.WarnLevel());
      else if (args == 1) obj->compiler.WarnLevel(duk_require_int(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::WarningLevel() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::WarningAsError(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->compiler.WarningAsError());
      else if (args == 1) obj->compiler.WarningAsError(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::WarningAsError() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::WarningDisable(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->compiler.WarningDisable());
      else obj->compiler.WarningDisable(JavaScriptHelper::AsIntVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::BeforeCompile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) {
         JavaScriptHelper::PushStashedCallback(duktapeContext, "JsLib::BeforeCompile", obj);
      }
      else if (args == 1) {
         JavaScriptHelper::StashCallback(duktapeContext, 0, "JsLib::BeforeCompile", obj);
         obj->compiler.BeforeCompile([=] () { JavaScriptHelper::CallStashedCallback(duktapeContext, "JsLib::BeforeCompile", obj); });
      }
      else {
         JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::BeforeCompile() expected");
      }

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::BeforeLink(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (!args) {
         JavaScriptHelper::PushStashedCallback(duktapeContext, "JsLib::BeforeLink", obj);
      }
      else if (args == 1) {
         JavaScriptHelper::StashCallback(duktapeContext, 0, "JsLib::BeforeLink", obj);
         obj->librarian.BeforeLink([=] () { JavaScriptHelper::CallStashedCallback(duktapeContext, "JsLib::BeforeLink", obj); });
      }
      else {
         JavaScriptHelper::Throw(duktapeContext, "One argument for Lib::BeforeLink() expected");
      }

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLib::Create(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLib>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Lib::Create() expected");

      obj->compiler.Compile();

      obj->librarian.Files(obj->compiler.ObjFiles());
      obj->librarian.DependencyCheck(obj->compiler.DependencyCheck());
      obj->librarian.Create();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

void JsLib::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsLib::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Lib");

   duk_pop(duktapeContext);
}

