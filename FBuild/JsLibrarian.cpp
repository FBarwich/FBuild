/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsLibrarian.h"


duk_ret_t JsLibrarian::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Librarian constructor expected");

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      duk_push_pointer(duktapeContext, new JsLibrarian);
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsLibrarian::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsLibrarian::Files, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Files");

      duk_push_c_function(duktapeContext, JsLibrarian::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsLibrarian::Output, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Output");

      duk_push_c_function(duktapeContext, JsLibrarian::BeforeLink, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "BeforeLink");

      duk_push_c_function(duktapeContext, JsLibrarian::Create, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Create");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLibrarian::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsLibrarian>(duktapeContext);
   return 0;
}

duk_ret_t JsLibrarian::Files(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLibrarian>(duktapeContext);

      if (!args) JavaScriptHelper::PushArray(duktapeContext, obj->librarian.Files());
      else obj->librarian.Files(JavaScriptHelper::AsStringVector(duktapeContext, args));

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLibrarian::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLibrarian>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->librarian.DependencyCheck());
      else if (args == 1) obj->librarian.DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Librarian::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLibrarian::Output(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLibrarian>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->librarian.Output().c_str());
      else if (args == 1) obj->librarian.Output(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for Librarian::Output() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLibrarian::BeforeLink(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLibrarian>(duktapeContext);

      if (!args) {
         JavaScriptHelper::PushStashedCallback(duktapeContext, "JsLibrarian::BeforeLink", obj);
      }
      else if (args == 1) {
         JavaScriptHelper::StashCallback(duktapeContext, 0, "JsLibrarian::BeforeLink", obj);
         obj->librarian.BeforeLink([=] () { JavaScriptHelper::CallStashedCallback(duktapeContext, "JsLibrarian::BeforeLink", obj); });
      }
      else {
         JavaScriptHelper::Throw(duktapeContext, "One argument for Librarian::BeforeLink() expected");
      }

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsLibrarian::Create(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsLibrarian>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for Librarian::Create() expected");

      obj->librarian.Create();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

void JsLibrarian::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsLibrarian::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Librarian");

   duk_pop(duktapeContext);
}

