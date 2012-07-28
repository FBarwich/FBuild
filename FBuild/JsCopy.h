/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include "JavaScriptHelper.h"
#include "Copy.h"

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

