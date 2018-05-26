/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "../Duktape/duktape.h"

class JavaScript {
   duk_context* duktapeContext;

   void SetArgs (const std::vector<std::string>& args);

   static duk_ret_t JsQuit (duk_context* duktapeContext);
   static duk_ret_t JsPrint (duk_context* duktapeContext);
   static duk_ret_t JsExecuteString (duk_context* duktapeContext);
   static duk_ret_t JsExecuteFile (duk_context* duktapeContext);
   static duk_ret_t JsSystem (duk_context* duktapeContext);
   static duk_ret_t JsFullPath(duk_context* duktapeContext);
   static duk_ret_t JsDelete(duk_context* duktapeContext);
   static duk_ret_t JsRun(duk_context* duktapeContext);
   static duk_ret_t JsTouch(duk_context* duktapeContext);
   static duk_ret_t JsGlob(duk_context* duktapeContext);
   static duk_ret_t JsBuild(duk_context* duktapeContext);
   static duk_ret_t JsFileOutOfDate(duk_context* duktapeContext);
   static duk_ret_t JsChangeDirectory(duk_context* duktapeContext);
   static duk_ret_t JsStringToFile(duk_context* duktapeContext);
   static duk_ret_t JsGetEnv(duk_context* duktapeContext);
   static duk_ret_t JsSetEnv(duk_context* duktapeContext);
   static duk_ret_t JsDirectorySync(duk_context* duktapeContext);
   static duk_ret_t JsToolChain(duk_context* duktapeContext);

public:
   JavaScript (const std::vector<std::string>& args);
   ~JavaScript ();

   void ExecuteString (const std::string& script, const std::string& = "string");
   void ExecuteFile (const std::filesystem::path& script);
};
