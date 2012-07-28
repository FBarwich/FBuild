/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JavaScriptHelper.h"

#include "Compiler.h"
#include "ResourceCompiler.h"
#include "Linker.h"

#pragma once
class JsExe : public JavaScriptHelper {
   Compiler compiler;
   ResourceCompiler resourceCompiler;
   Linker linker;

   static void Register (const char* classname, v8::Handle<v8::ObjectTemplate>& global);

   static v8::Handle<v8::Value> GetSet (const v8::Arguments& args);
   static v8::Handle<v8::Value> Create (const v8::Arguments& args);

public:
   static void Register (v8::Handle<v8::ObjectTemplate>& global);
};

