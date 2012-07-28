#pragma once

#include "JavaScriptHelper.h"
#include "FileToCpp.h"

class JsFileToCpp : public JavaScriptHelper {
   FileToCpp file2cpp;

   static v8::Handle<v8::Value> GetSet (const v8::Arguments& args);
   static v8::Handle<v8::Value> Create (const v8::Arguments& args);

public:

   static void Register (v8::Handle<v8::ObjectTemplate>& global);
};

