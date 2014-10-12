/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "Precompiled.h"

#include "JavaScript.h"
#include "JavaScriptHelper.h"

#include "FileOutOfDate.h"

#include "JsCopy.h"
#include "JsLib.h"
#include "JsCompiler.h"
#include "JsLibrarian.h"
#include "JsExe.h"

#include <Shlwapi.h>


JavaScript::JavaScript (const std::vector<std::string>& args)
{
   duktapeContext = duk_create_heap_default();
   if (!duktapeContext) throw std::runtime_error("Unable to create JavaScript-Context");

   SetArgs(args);

   duk_push_global_object(duktapeContext);

   duk_push_c_function(duktapeContext, JsQuit, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Quit");

   duk_push_c_function(duktapeContext, JsPrint, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Print");

   duk_push_c_function(duktapeContext, JsExecuteString, 2);
   duk_put_prop_string(duktapeContext, -2, "ExecuteString");

   duk_push_c_function(duktapeContext, JsExecuteFile, 1);
   duk_put_prop_string(duktapeContext, -2, "ExecuteFile");

   duk_push_c_function(duktapeContext, JsSystem, 1);
   duk_put_prop_string(duktapeContext, -2, "System");

   duk_push_c_function(duktapeContext, JsFullPath, 1);
   duk_put_prop_string(duktapeContext, -2, "FullPath");

   duk_push_c_function(duktapeContext, JsDelete, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Delete");

   duk_push_c_function(duktapeContext, JsRun, 2);
   duk_put_prop_string(duktapeContext, -2, "Run");

   duk_push_c_function(duktapeContext, JsTouch, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Touch");

   duk_push_c_function(duktapeContext, JsGlob, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Glob");

   duk_push_c_function(duktapeContext, JsBuild, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "Build");

   duk_push_c_function(duktapeContext, JsFileOutOfDate, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "FileOutOfDate");

   duk_push_c_function(duktapeContext, JsChangeDirectory, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "ChangeDirectory");

   duk_push_c_function(duktapeContext, JsStringToFile, DUK_VARARGS);
   duk_put_prop_string(duktapeContext, -2, "StringToFile");

   duk_push_c_function(duktapeContext, JsGetEnv, 1);
   duk_put_prop_string(duktapeContext, -2, "GetEnv");

   duk_push_c_function(duktapeContext, JsSetEnv, 2);
   duk_put_prop_string(duktapeContext, -2, "SetEnv");

   duk_pop(duktapeContext);

   JsCopy::Register(duktapeContext);
   JsLib::Register(duktapeContext);
   JsCompiler::Register(duktapeContext);
   JsLibrarian::Register(duktapeContext);
   JsExe::Register(duktapeContext);
   //JsLinker::Register(duktapeContext);
   //JsResourceCompiler::Register(duktapeContext);
   //JsFileToCpp::Register(duktapeContext);
}

JavaScript::~JavaScript ()
{
   if (duktapeContext) duk_destroy_heap(duktapeContext);
}

void JavaScript::SetArgs (const std::vector<std::string>& args)
{
   duk_push_global_object(duktapeContext);
   duk_push_object(duktapeContext);

   for (const auto& arg : args) {
      auto pos = arg.find(':');
      if (pos == std::string::npos) pos = arg.find('=');
      if (pos == std::string::npos) {
         duk_push_string(duktapeContext, "");
         duk_put_prop_string(duktapeContext, -2, arg.c_str());
      }
      else {
         std::string key = arg.substr(0, pos);
         std::string val = arg.substr(pos + 1);
         duk_push_string(duktapeContext, val.c_str());
         duk_put_prop_string(duktapeContext, -2, key.c_str());
      }
   }

   duk_put_prop_string(duktapeContext, -2, "args");
   duk_pop(duktapeContext);
}

void JavaScript::ExecuteString (const std::string& code, const std::string& name)
{
   duk_push_string(duktapeContext, code.c_str());
   duk_push_string(duktapeContext, name.c_str());

   if (duk_pcompile(duktapeContext, DUK_COMPILE_NORESULT)) {
      std::string error = duk_safe_to_string(duktapeContext, -1);
      throw std::runtime_error(error);
   }

   if (duk_pcall(duktapeContext, 0)) {
      std::string error = duk_safe_to_string(duktapeContext, -1);
      throw std::runtime_error(error);
   }
}


void JavaScript::ExecuteFile (const boost::filesystem::path& script)
{
   if (!boost::filesystem::exists(script)) throw std::runtime_error("File " + script.string() + " does not exist");

   boost::filesystem::path file = boost::filesystem::canonical(script);
   file.make_preferred();

   std::string str = file.string();
   if (duk_peval_file(duktapeContext, str.c_str())) {
      std::string error = duk_safe_to_string(duktapeContext, -1);
      throw std::runtime_error(error);
   }

   duk_pop(duktapeContext);
}

duk_ret_t JavaScript::JsQuit (duk_context* duktapeContext)
{
   if (duk_get_top(duktapeContext) != 1) JavaScriptHelper::Throw(duktapeContext, "One argument for Quit() expected");
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "Quit() can't be constructed");

   int exitCode = duk_to_int(duktapeContext, 0);
   exit(exitCode);
}

duk_ret_t JavaScript::JsPrint (duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "Print() can't be constructed");

   int top = duk_get_top(duktapeContext);
   for (int i = 0; i < top; ++i) {
      std::cout << duk_to_string(duktapeContext, i) << " ";
   }

   std::cout << "\n";

   return 0;
}

duk_ret_t JavaScript::JsExecuteString(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "ExecuteString() can't be constructed");

   std::string code = duk_require_string(duktapeContext, 0);
   std::string name = duk_to_string(duktapeContext, 1);
   if (name == "undefined") name = "<ExecuteString>";

   duk_push_string(duktapeContext, code.c_str());
   duk_push_string(duktapeContext, name.c_str());

   duk_compile(duktapeContext, DUK_COMPILE_NORESULT);
   duk_call(duktapeContext, 0);

   return 0;
}

duk_ret_t JavaScript::JsExecuteFile(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "ExecuteFile() can't be constructed");

   std::string script = duk_require_string(duktapeContext, 0);

   if (!boost::filesystem::exists(script)) JavaScriptHelper::Throw(duktapeContext, "File " + script + " does not exist");

   boost::filesystem::path file = boost::filesystem::canonical(script);
   file.make_preferred();

   std::string str = file.string();
   duk_eval_file_noresult(duktapeContext, str.c_str());

   return 0;
}

duk_ret_t JavaScript::JsSystem(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "System() can't be constructed");

   std::string command = duk_require_string(duktapeContext, 0);

   int rc = std::system(command.c_str());

   duk_push_int(duktapeContext, rc);
   return 1;
}

duk_ret_t JavaScript::JsFullPath(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "FullPath() can't be constructed");

   std::string path = duk_require_string(duktapeContext, 0);

   auto full = boost::filesystem::canonical(path);
   full.make_preferred();

   std::string str = full.string();
   duk_push_string(duktapeContext, str.c_str());
   return 1;
}

duk_ret_t JavaScript::JsDelete(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "Delete() can't be constructed");

   const std::vector<std::string> files = JavaScriptHelper::AsStringVector(duktapeContext);
   if (files.empty()) JavaScriptHelper::Throw(duktapeContext, "Filename(s) for Delete() expected");

   try {
      for (const auto& file : files) {
         boost::system::error_code rc;
         boost::filesystem::remove_all(file, rc);
      }
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }

   return 0;
}

duk_ret_t JavaScript::JsRun(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "Run() can't be constructed");

   std::string command = duk_require_string(duktapeContext, 0);
   bool catchOutput = duk_to_boolean(duktapeContext, 1) != false;

   auto tmpfile = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

   if (catchOutput) command += " 1>" + tmpfile.string() + " 2>&1";

   int rc = std::system(command.c_str());
   if (rc) JavaScriptHelper::Throw(duktapeContext, "Error running command " + command);

   if (catchOutput) {
      {
         std::ifstream in(tmpfile.string());
         std::stringstream ss;
         ss << in.rdbuf();

         std::string r = ss.str();
         boost::trim_right(r);

         duk_push_string(duktapeContext, r.c_str());
      }

      boost::filesystem::remove(tmpfile);
      return 1;
   }
   else {
      return 0;
   }
}

duk_ret_t JavaScript::JsTouch(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "Touch() can't be constructed");

   const std::vector<std::string> files = JavaScriptHelper::AsStringVector(duktapeContext);
   if (files.empty()) JavaScriptHelper::Throw(duktapeContext, "Filename(s) for Touch() expected");

   try {
      for (const auto& filename : files) {
         if (!boost::filesystem::exists(filename)) throw std::exception(std::string("File " + filename + " does not exist").c_str());
         if (!boost::filesystem::is_regular_file(filename)) throw std::exception(std::string(filename + " is not a file").c_str());
         boost::filesystem::last_write_time(filename, std::time(nullptr));
      }
   }
   catch (std::exception& e) {
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }

   return 0;
}

duk_ret_t JavaScript::JsGlob(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "Glob() can't be constructed");

   std::string path = ".";
   std::string pattern = "*";

   int args = duk_get_top(duktapeContext);
   if (args == 1) {
      pattern = duk_require_string(duktapeContext, 0);
   }
   else if (args == 2) {
      path = duk_require_string(duktapeContext, 0);
      pattern = duk_require_string(duktapeContext, 1);
   }
   else {
      JavaScriptHelper::Throw(duktapeContext, "Expected one or two arguments for Glob()");
   }

   std::vector<std::string> files;
   files.reserve(250);

   if (boost::filesystem::exists(path)) {
      std::for_each(boost::filesystem::directory_iterator(path), boost::filesystem::directory_iterator(), [&] (const boost::filesystem::directory_entry& entry) {
         if (boost::filesystem::is_regular_file(entry.path())) {
            if (PathMatchSpec(entry.path().filename().string().c_str(), pattern.c_str())) {
               files.push_back(boost::filesystem::canonical(entry.path()).make_preferred().string());
            }
         }
      });
   }

   duk_push_array(duktapeContext);

   for (size_t i = 0; i < files.size(); ++i) {
      duk_push_string(duktapeContext, files[i].c_str());
      duk_put_prop_index(duktapeContext, -2, i);
   }

   return 1;
}

duk_ret_t JavaScript::JsBuild(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "Build() can't be constructed");

   if (duk_get_top(duktapeContext) != 1) JavaScriptHelper::Throw(duktapeContext, "One argument for Build() expected");

   const auto current = boost::filesystem::current_path();
   boost::filesystem::current_path(duk_require_string(duktapeContext, 0));

   try {
      boost::filesystem::path file = boost::filesystem::canonical("FBuild.js");
      file.make_preferred();

      const std::string str = file.string();
      duk_eval_file_noresult(duktapeContext, str.c_str());
   }
   catch (std::exception& e) {
      boost::filesystem::current_path(current);
      JavaScriptHelper::Throw(duktapeContext, e.what());
   }

   boost::filesystem::current_path(current);

   return 0;
}

duk_ret_t JavaScript::JsFileOutOfDate(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "FileOutOfDate() can't be constructed");

   const auto files = JavaScriptHelper::AsStringVector(duktapeContext);
   if (files.size() < 2) JavaScriptHelper::Throw(duktapeContext, "Expected two or more arguments for FileOutOfDate()");

   FileOutOfDate outOfDate;
   outOfDate.Parent(files[0]);

   for (size_t i = 1; i < files.size(); ++i) {
      outOfDate.AddFile(files[i]);
   }

   duk_push_boolean(duktapeContext, outOfDate.Go());
   return 1;
}

duk_ret_t JavaScript::JsChangeDirectory(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "ChangeDirectory() can't be constructed");

   if (duk_get_top(duktapeContext) != 1) JavaScriptHelper::Throw(duktapeContext, "One arguments for ChangeDirectory() expected");

   boost::filesystem::current_path(duk_require_string(duktapeContext, 0));
   return 0;
}

duk_ret_t JavaScript::JsStringToFile(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "StringToFile() can't be constructed");

   if (duk_get_top(duktapeContext) != 2) JavaScriptHelper::Throw(duktapeContext, "Two arguments for StringToFile() expected");

   std::string file = duk_require_string(duktapeContext, 0);
   std::string content = duk_to_string(duktapeContext, 1);

   std::ofstream out(file, std::ofstream::trunc);
   if (out.fail()) JavaScriptHelper::Throw(duktapeContext, "Error opening " + file);

   out << content;

   return 0;
}

duk_ret_t JavaScript::JsGetEnv(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "GetEnv() can't be constructed");

   const char* env = std::getenv(duk_require_string(duktapeContext, 0));
   if (env) {
      duk_push_string(duktapeContext, env);
      return 1;
   }
   else {
      return 0;
   }
}

duk_ret_t JavaScript::JsSetEnv(duk_context* duktapeContext)
{
   if (duk_is_constructor_call(duktapeContext)) JavaScriptHelper::Throw(duktapeContext, "SetEnv() can't be constructed");

   std::string arg = duk_require_string(duktapeContext, 0);
   arg += "=";
   arg += duk_require_string(duktapeContext, 1);

   int rc = _putenv(arg.c_str());
   if (rc) JavaScriptHelper::Throw(duktapeContext, "Error putting environment " + arg);

   return 0;
}
