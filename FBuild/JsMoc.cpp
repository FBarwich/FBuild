/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsMoc.h"

void JsMoc::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsMoc::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Moc");
   duk_pop(duktapeContext);
}

duk_ret_t JsMoc::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 1) JavaScriptHelper::Throw(duktapeContext, "Expected path to QTs bin directory as argument for Moc-Constructor");

      std::string qtBin = duk_require_string(duktapeContext, 0);

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      duk_push_pointer(duktapeContext, new JsMoc{qtBin});
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsMoc::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsMoc::Files, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Files");

      duk_push_c_function(duktapeContext, JsMoc::OutDir, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "OutDir");

      duk_push_c_function(duktapeContext, JsMoc::Prefix, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Prefix");

      duk_push_c_function(duktapeContext, JsMoc::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsMoc::Compile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Compile");

      duk_push_c_function(duktapeContext, JsMoc::OutFiles, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "OutFiles");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsMoc::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsMoc>(duktapeContext);
   return 0;
}

duk_ret_t JsMoc::Files(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsMoc>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->moc_->Files());
      else obj->moc_->Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsMoc::OutDir(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsMoc>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->moc_->OutDir().c_str());
      else if (args == 1) obj->moc_->OutDir(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "Expected one argument for Moc::Outdir()");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsMoc::Prefix(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsMoc>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->moc_->Prefix().c_str());
      else if (args == 1) obj->moc_->Prefix(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "Expected one argument for Moc::Outdir()");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsMoc::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsMoc>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->moc_->DependencyCheck());
      else if (args == 1) obj->moc_->DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Moc::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsMoc::Compile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsMoc>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Moc::Compile() expected");

      obj->moc_->Compile();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsMoc::OutFiles(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);
      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Moc::OutFiles() expected");

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsMoc>(duktapeContext);

      JavaScriptHelper::PushArray(duktapeContext, obj->moc_->OutFiles());

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}
