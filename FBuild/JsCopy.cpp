/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsCopy.h"




duk_ret_t JsCopy::Function(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) return Constructor(duktapeContext);
   else return Standalone(duktapeContext);
}

duk_ret_t JsCopy::Standalone(duk_context* duktapeContext)
{
   int args = duk_get_top(duktapeContext);
   if (args < 2) JavaScriptHelper::Throw(duktapeContext, "Expected two or three arguments for Copy()");

   Copy copy;
   copy.Source(duk_require_string(duktapeContext, 0));
   copy.Dest(duk_require_string(duktapeContext, 1));
   if (args > 2) copy.DependencyCheck(duk_to_boolean(duktapeContext, 2) != 0);

   int copied = copy.Go();

   duk_push_int(duktapeContext, copied);
   return 1;
}

duk_ret_t JsCopy::Constructor(duk_context* duktapeContext)
{
   if (duk_get_top(duktapeContext) != 0) JavaScriptHelper::Throw(duktapeContext, "No arguments for constructor for Copy expected");

   JsCopy* copy = new JsCopy;

   duk_push_this(duktapeContext);

   duk_push_pointer(duktapeContext, copy);
   duk_put_prop_string(duktapeContext, -2, "__Ptr");

   duk_push_c_function(duktapeContext, JsCopy::Destructor, 1);
   duk_set_finalizer(duktapeContext, -2);

   duk_push_c_function(duktapeContext, JsCopy::Source, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Source");

   duk_push_c_function(duktapeContext, JsCopy::Dest, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Dest");

   duk_push_c_function(duktapeContext, JsCopy::DependencyCheck, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "DependencyCheck");

   duk_push_c_function(duktapeContext, JsCopy::NeedsCopy, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "NeedsCopy");

   duk_push_c_function(duktapeContext, JsCopy::Go, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Go");

   return 1;
}

duk_ret_t JsCopy::Destructor(duk_context* duktapeContext)
{
   delete JavaScriptHelper::CppObject<JsCopy>(duktapeContext);
   return 0;
}

duk_ret_t JsCopy::Source(duk_context* duktapeContext)
{
   int args = duk_get_top(duktapeContext);

   duk_push_this(duktapeContext);
   JsCopy* obj = JavaScriptHelper::CppObject<JsCopy>(duktapeContext);

   if (!args) {
      duk_push_string(duktapeContext, obj->copy.Source().c_str());
      return 1;
   }
   else if (args == 1) {
      obj->copy.Source(duk_require_string(duktapeContext, 0));
      return 1;
   }
   else {
      JavaScriptHelper::Throw(duktapeContext, "Expected one argument for Copy::Source()");
   }
}

duk_ret_t JsCopy::Dest(duk_context* duktapeContext)
{
   int args = duk_get_top(duktapeContext);

   duk_push_this(duktapeContext);
   JsCopy* obj = JavaScriptHelper::CppObject<JsCopy>(duktapeContext);

   if (!args) {
      duk_push_string(duktapeContext, obj->copy.Dest().c_str());
      return 1;
   }
   else if (args == 1) {
      obj->copy.Dest(duk_require_string(duktapeContext, 0));
      return 1;
   }
   else {
      JavaScriptHelper::Throw(duktapeContext, "Expected one argument for Copy::Dest()");
   }
}

duk_ret_t JsCopy::DependencyCheck(duk_context* duktapeContext)
{
   int args = duk_get_top(duktapeContext);

   duk_push_this(duktapeContext);
   JsCopy* obj = JavaScriptHelper::CppObject<JsCopy>(duktapeContext);

   if (!args) {
      duk_push_boolean(duktapeContext, obj->copy.DependencyCheck());
      return 1;
   }
   else if (args == 1) {
      obj->copy.DependencyCheck(duk_require_boolean(duktapeContext, 0) != 0);
      return 1;
   }
   else {
      JavaScriptHelper::Throw(duktapeContext, "Expected one argument for Copy::DependencyCheck()");
   }
}

duk_ret_t JsCopy::NeedsCopy(duk_context* duktapeContext)
{
   int args = duk_get_top(duktapeContext);

   duk_push_this(duktapeContext);
   JsCopy* obj = JavaScriptHelper::CppObject<JsCopy>(duktapeContext);

   if (!args) {
      duk_push_boolean(duktapeContext, obj->copy.NeedsCopy());
      return 1;
   }
   else {
      JavaScriptHelper::Throw(duktapeContext, "Expected no arguments for Copy::NeedsCopy()");
   }
}

duk_ret_t JsCopy::Go(duk_context* duktapeContext)
{
   int args = duk_get_top(duktapeContext);

   duk_push_this(duktapeContext);
   JsCopy* obj = JavaScriptHelper::CppObject<JsCopy>(duktapeContext);

   if (!args) {
      int copied = obj->copy.Go();
      duk_push_int(duktapeContext, copied);
      return 1;
   }
   else {
      JavaScriptHelper::Throw(duktapeContext, "Expected no arguments for Copy::Go()");
   }
}

void JsCopy::Register(duk_context* duktapeContext)
{
   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsCopy::Function, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Copy");

   duk_pop(duktapeContext);
}

