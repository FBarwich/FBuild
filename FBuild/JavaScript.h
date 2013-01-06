/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <v8.h>

#include <string>
#include <vector>


class JavaScript {
   v8::Persistent<v8::Context> context;

   void SetArgs (v8::Handle<v8::ObjectTemplate>& global, const std::vector<std::string>& args);

   static void CheckException (v8::TryCatch& tryCatch);

   static v8::Handle<v8::Value> JsExecuteString (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsExecuteFile (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsPrint (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsQuit (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsRun (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsSystem (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsGlob (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsFullPath (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsBuild (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsFileOutOfDate (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsChangeDirectory (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsStringToFile (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsGetEnv (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsSetEnv (const v8::Arguments& args);
   static v8::Handle<v8::Value> JsTouch (const v8::Arguments& args);

   static void Exec (const char* code, size_t len, const std::string& name);

public:
   JavaScript (const std::vector<std::string>& args);
   ~JavaScript ();

   void ExecuteBuffer (const char* code, size_t len, const std::string& name = "buffer");
   void ExecuteString (const std::string& script, const std::string& = "string");
   void ExecuteFile (const std::string& script);
};

