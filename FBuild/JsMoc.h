/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include "Moc.h"
#include "JavaScriptHelper.h"

#include <memory>

class JsMoc {
public:
   JsMoc (const std::string& qtBin) : moc_{std::make_unique<Moc>(qtBin)} {}

   static void Register (duk_context* duktapeContext);

private:
   std::unique_ptr<Moc> moc_;

   static duk_ret_t Constructor(duk_context* duktapeContext);
   static duk_ret_t Destructor(duk_context* duktapeContext);
   static duk_ret_t Files(duk_context* duktapeContext);
   static duk_ret_t OutDir(duk_context* duktapeContext);
   static duk_ret_t Prefix(duk_context* duktapeContext);
   static duk_ret_t DependencyCheck(duk_context* duktapeContext);

   static duk_ret_t Compile(duk_context* duktapeContext);

   static duk_ret_t OutFiles(duk_context* duktapeContext);
};

