/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsUic.h"

void JsUic::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsUic::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Uic");
   duk_pop(duktapeContext);
}

duk_ret_t JsUic::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 1) JavaScriptHelper::Throw(duktapeContext, "Expected path to QTs bin directory as argument for Uic-Constructor");

      std::string qtBin = duk_require_string(duktapeContext, 0);

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      duk_push_pointer(duktapeContext, new JsUic{qtBin});
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsUic::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsUic::Files, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Files");

      duk_push_c_function(duktapeContext, JsUic::OutDir, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "OutDir");

      duk_push_c_function(duktapeContext, JsUic::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsUic::Compile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Compile");

      duk_push_c_function(duktapeContext, JsUic::OutFiles, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "OutFiles");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsUic::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsUic>(duktapeContext);
   return 0;
}

duk_ret_t JsUic::Files(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsUic>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->uic_->Files());
      else obj->uic_->Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsUic::OutDir(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsUic>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->uic_->OutDir().c_str());
      else if (args == 1) obj->uic_->OutDir(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "Expected one argument for Uic::Outdir()");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsUic::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsUic>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->uic_->DependencyCheck());
      else if (args == 1) obj->uic_->DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Uic::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsUic::Compile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsUic>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Uic::Compile() expected");

      obj->uic_->Compile();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsUic::OutFiles(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);
      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Uic::OutFiles() expected");

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsUic>(duktapeContext);

      JavaScriptHelper::PushArray(duktapeContext, obj->uic_->OutFiles());

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}
