
#include "JsFileToCpp.h"



duk_ret_t JsFileToCpp::Constructor(duk_context* duktapeContext)
{
   try {
      if (duk_get_top(duktapeContext) != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for FileToCpp constructor expected");

      if (duk_is_constructor_call(duktapeContext)) duk_push_this(duktapeContext);
      else duk_push_object(duktapeContext);

      duk_push_pointer(duktapeContext, new JsFileToCpp);
      duk_put_prop_string(duktapeContext, -2, "__Ptr");

      duk_push_c_function(duktapeContext, JsFileToCpp::Destructor, 1);
      duk_set_finalizer(duktapeContext, -2);

      duk_push_c_function(duktapeContext, JsFileToCpp::DependencyCheck, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

      duk_push_c_function(duktapeContext, JsFileToCpp::Infile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Infile");

      duk_push_c_function(duktapeContext, JsFileToCpp::Outfile, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Outfile");

      duk_push_c_function(duktapeContext, JsFileToCpp::NullTerminator, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "NullTerminator");

      duk_push_c_function(duktapeContext, JsFileToCpp::Namespace, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Namespace");

      duk_push_c_function(duktapeContext, JsFileToCpp::Ptr, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Ptr");

      duk_push_c_function(duktapeContext, JsFileToCpp::Intro, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Intro");

      duk_push_c_function(duktapeContext, JsFileToCpp::Outro, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Outro");

      duk_push_c_function(duktapeContext, JsFileToCpp::Additional, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Additional");

      duk_push_c_function(duktapeContext, JsFileToCpp::Array, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Array");

      duk_push_c_function(duktapeContext, JsFileToCpp::Const, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Const");

      duk_push_c_function(duktapeContext, JsFileToCpp::Create, DUK_VARARGS);
      duk_put_prop_string(duktapeContext, -2, "Create");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);
   return 0;
}

duk_ret_t JsFileToCpp::DependencyCheck(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->fileToCpp.DependencyCheck());
      else if (args == 1) obj->fileToCpp.DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::DependencyCheck() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Infile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->fileToCpp.Infile().c_str());
      else if (args == 1) obj->fileToCpp.Infile(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Infile() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Outfile(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->fileToCpp.Outfile().c_str());
      else if (args == 1) obj->fileToCpp.Outfile(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Outfile() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::NullTerminator(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->fileToCpp.NullTerminator());
      else if (args == 1) obj->fileToCpp.NullTerminator(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::NullTerminator() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Namespace(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->fileToCpp.Namespace().c_str());
      else if (args == 1) obj->fileToCpp.Namespace(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Namespace() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Ptr(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->fileToCpp.Ptr().c_str());
      else if (args == 1) obj->fileToCpp.Ptr(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Ptr() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Intro(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->fileToCpp.Intro().c_str());
      else if (args == 1) obj->fileToCpp.Intro(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Intro() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Outro(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->fileToCpp.Outro().c_str());
      else if (args == 1) obj->fileToCpp.Outro(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Outro() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Additional(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->fileToCpp.Additional().c_str());
      else if (args == 1) obj->fileToCpp.Additional(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Additional() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Array(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_string(duktapeContext, obj->fileToCpp.Array().c_str());
      else if (args == 1) obj->fileToCpp.Array(duk_require_string(duktapeContext, 0));
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Array() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Const(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (!args) duk_push_boolean(duktapeContext, obj->fileToCpp.Const());
      else if (args == 1) obj->fileToCpp.Const(duk_require_boolean(duktapeContext, 0) != 0);
      else JavaScriptHelper::Throw(duktapeContext, "One argument for FileToCpp::Const() expected");

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

duk_ret_t JsFileToCpp::Create(duk_context* duktapeContext)
{
   try {
      int args = duk_get_top(duktapeContext);

      duk_push_this(duktapeContext);
      auto obj = JavaScriptHelper::CppObject<JsFileToCpp>(duktapeContext);

      if (args != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for FileToCpp::Create() expected");

      obj->fileToCpp.Create();

      return 1;
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }
}

void JsFileToCpp::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsFileToCpp::Constructor, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "FileToCpp");

   duk_pop(duktapeContext);
}
