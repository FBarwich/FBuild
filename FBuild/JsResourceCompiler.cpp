/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsResourceCompiler.h"



duk_ret_t JsResourceCompiler::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for ResourceCompiler constructor expected");

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      duk_push_pointer(duktapeContext, new JsResourceCompiler);
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsResourceCompiler::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsResourceCompiler::Files, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Files");

      duk_push_c_function(duktapeContext, JsResourceCompiler::Outdir, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Outdir");

      duk_push_c_function(duktapeContext, JsResourceCompiler::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsResourceCompiler::Outfiles, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Outfiles");

      duk_push_c_function(duktapeContext, JsResourceCompiler::Compile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Compile");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsResourceCompiler::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsResourceCompiler>(duktapeContext);
   return 0;
}

duk_ret_t JsResourceCompiler::Files(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsResourceCompiler>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->resourceCompiler.Files());
      else obj->resourceCompiler.Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsResourceCompiler::Outdir(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsResourceCompiler>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->resourceCompiler.Outdir().c_str());
      else if (args == 1) obj->resourceCompiler.Outdir(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for ResourceCompiler::Outdir() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsResourceCompiler::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsResourceCompiler>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->resourceCompiler.DependencyCheck());
      else if (args == 1) obj->resourceCompiler.DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for ResourceCompiler::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsResourceCompiler::Outfiles(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsResourceCompiler>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->resourceCompiler.Outfiles());
      else JavaScriptHelper::Throw(duktapeContext, "No arguments for ResourceCompiler::Outfiles() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsResourceCompiler::Compile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsResourceCompiler>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for ResourceCompiler::Compile() expected");

      obj->resourceCompiler.Compile();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

void JsResourceCompiler::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsResourceCompiler::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "ResourceCompiler");
   duk_pop(duktapeContext);
}
