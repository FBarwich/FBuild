/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include "Copy.h"

#include "JavaScriptHelper.h"


class JsCopy {
   Copy copy;

   static duk_ret_t Function(duk_context* duktapeContext);
   static duk_ret_t Standalone(duk_context* duktapeContext);

   static duk_ret_t Constructor(duk_context* duktapeContext);
   static duk_ret_t Destructor(duk_context* duktapeContext);
   static duk_ret_t Source(duk_context* duktapeContext);
   static duk_ret_t Dest(duk_context* duktapeContext);
   static duk_ret_t DependencyCheck(duk_context* duktapeContext);
   static duk_ret_t NeedsCopy(duk_context* duktapeContext);
   static duk_ret_t Go(duk_context* duktapeContext);

public:

   static void Register(duk_context* duktapeContext);
};


/* TODO

class JsCopy : public JavaScriptHelper {
   Copy copy;

   static v8::Handle<v8::Value> CopyFunc (const v8::Arguments& args);

   static v8::Handle<v8::Value> Construct (const v8::Arguments& args);
   static v8::Handle<v8::Value> GetSet (const v8::Arguments& args);
   static v8::Handle<v8::Value> NeedsCopy (const v8::Arguments& args);
   static v8::Handle<v8::Value> Go (const v8::Arguments& args);

public:

   static void Register (v8::Handle<v8::ObjectTemplate>& global);
};

*/