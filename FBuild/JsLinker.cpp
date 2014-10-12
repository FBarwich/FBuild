/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsLinker.h"


duk_ret_t JsLinker::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Exe/Dll constructor expected");

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      duk_push_pointer(duktapeContext, new JsLinker);
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsLinker::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsLinker::Build, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Build");

      duk_push_c_function(duktapeContext, JsLinker::Files, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Files");

      duk_push_c_function(duktapeContext, JsLinker::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsLinker::Output, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Output");

      duk_push_c_function(duktapeContext, JsLinker::LibPath, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "LibPath");

      duk_push_c_function(duktapeContext, JsLinker::Libs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Libs");

      duk_push_c_function(duktapeContext, JsLinker::ImportLib, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "ImportLib");

      duk_push_c_function(duktapeContext, JsLinker::Def, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Def");

      duk_push_c_function(duktapeContext, JsLinker::LinkArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "LinkArgs");

      duk_push_c_function(duktapeContext, JsLinker::LinkArgs, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Args");

      duk_push_c_function(duktapeContext, JsLinker::BeforeLink, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "BeforeLink");

      duk_push_c_function(duktapeContext, JsLinker::Link, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Link");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsLinker>(duktapeContext);
   return 0;
}

duk_ret_t JsLinker::Build(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.Build().c_str());
      else if (args == 1) obj->linker.Build(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Linker::Build() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::Output(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.Output().c_str());
      else if (args == 1) obj->linker.Output(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Linker::Output() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::Files(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->linker.Files());
      else obj->linker.Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->linker.DependencyCheck());
      else if (args == 1) obj->linker.DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Linker::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::LibPath(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->linker.Libpath());
      else obj->linker.Libpath(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::Libs(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->linker.Libs());
      else obj->linker.Libs(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::ImportLib(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.ImportLib().c_str());
      else if (args == 1) obj->linker.ImportLib(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Linker::ImportLib() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::Def(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.Def().c_str());
      else if (args == 1) obj->linker.Def(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Linker::Def() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::LinkArgs(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->linker.Args().c_str());
      else obj->linker.Args(JavaScriptHelper::AsString(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::BeforeLink(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (!args) {
         JavaScriptHelper::PushStashedCallback(duktapeContext, "JsLinker::BeforeLink", obj);
      }
      else if (args == 1) {
         JavaScriptHelper::StashCallback(duktapeContext, 0, "JsLinker::BeforeLink", obj);
         obj->linker.BeforeLink([=] () { JavaScriptHelper::CallStashedCallback(duktapeContext, "JsLinker::BeforeLink", obj); });
      }
      else {
         JavaScriptHelper::Throw(duktapeContext, "One argument for Linker::BeforeLink() expected");
      }

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLinker::Link(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLinker>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Linker::Link() expected");

      obj->linker.Link();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

void JsLinker::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsLinker::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Linker");

   duk_pop(duktapeContext);
}

