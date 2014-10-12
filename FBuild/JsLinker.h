/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include "Linker.h"

#include "JavaScriptHelper.h"

class JsLinker {
   Linker linker;

   static duk_ret_t Constructor(duk_context* duktapeContext);
   static duk_ret_t Destructor(duk_context* duktapeContext);
   static duk_ret_t Build(duk_context* duktapeContext);
   static duk_ret_t Output(duk_context* duktapeContext);
   static duk_ret_t Files(duk_context* duktapeContext);
   static duk_ret_t DependencyCheck(duk_context* duktapeContext);
   static duk_ret_t LibPath(duk_context* duktapeContext);
   static duk_ret_t Libs(duk_context* duktapeContext);
   static duk_ret_t ImportLib(duk_context* duktapeContext);
   static duk_ret_t Def(duk_context* duktapeContext);
   static duk_ret_t LinkArgs(duk_context* duktapeContext);

   static duk_ret_t BeforeLink(duk_context* duktapeContext);

   static duk_ret_t Link(duk_context* duktapeContext);

public:

   static void Register(duk_context* duktapeContext);
};

