#pragma once

#include "FileToCpp.h"

#include "JavaScriptHelper.h"

class JsFileToCpp {
   FileToCpp fileToCpp;

   static duk_ret_t Constructor(duk_context* duktapeContext);
   static duk_ret_t Destructor(duk_context* duktapeContext);
   static duk_ret_t DependencyCheck(duk_context* duktapeContext);
   static duk_ret_t Infile(duk_context* duktapeContext);
   static duk_ret_t Outfile(duk_context* duktapeContext);
   static duk_ret_t NullTerminator(duk_context* duktapeContext);
   static duk_ret_t Namespace(duk_context* duktapeContext);
   static duk_ret_t Ptr(duk_context* duktapeContext);
   static duk_ret_t Intro(duk_context* duktapeContext);
   static duk_ret_t Outro(duk_context* duktapeContext);
   static duk_ret_t Additional(duk_context* duktapeContext);
   static duk_ret_t Array(duk_context* duktapeContext);
   static duk_ret_t Const(duk_context* duktapeContext);

   static duk_ret_t Create(duk_context* duktapeContext);

public:

   static void Register(duk_context* duktapeContext);
};
