/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JavaScript.h"
#include "JavaScriptHelper.h"

#include "JsCopy.h"
#include "JsCompiler.h"
#include "JsLinker.h"
#include "JsExe.h"
#include "JsResourceCompiler.h"
#include "JsLibrarian.h"
#include "JsLib.h"
#include "JsFileToCpp.h"
#include "FileOutOfDate.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <Shlwapi.h>




JavaScript::JavaScript (const std::vector<std::string>& args)
{
   v8::HandleScope scope;

   v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

   global->Set(v8::String::New("ExecuteString"), v8::FunctionTemplate::New(JsExecuteString));
   global->Set(v8::String::New("ExecuteFile"), v8::FunctionTemplate::New(JsExecuteFile));
   global->Set(v8::String::New("Print"), v8::FunctionTemplate::New(JsPrint));
   global->Set(v8::String::New("Quit"), v8::FunctionTemplate::New(JsQuit));
   global->Set(v8::String::New("Run"), v8::FunctionTemplate::New(JsRun));
   global->Set(v8::String::New("System"), v8::FunctionTemplate::New(JsSystem));
   global->Set(v8::String::New("Glob"), v8::FunctionTemplate::New(JsGlob));
   global->Set(v8::String::New("FullPath"), v8::FunctionTemplate::New(JsFullPath));
   global->Set(v8::String::New("Build"), v8::FunctionTemplate::New(JsBuild));
   global->Set(v8::String::New("FileOutOfDate"), v8::FunctionTemplate::New(JsFileOutOfDate));
   global->Set(v8::String::New("ChangeDirectory"), v8::FunctionTemplate::New(JsChangeDirectory));
   global->Set(v8::String::New("StringToFile"), v8::FunctionTemplate::New(JsStringToFile));
   global->Set(v8::String::New("GetEnv"), v8::FunctionTemplate::New(JsGetEnv));
   global->Set(v8::String::New("SetEnv"), v8::FunctionTemplate::New(JsSetEnv));
   global->Set(v8::String::New("Touch"), v8::FunctionTemplate::New(JsTouch));
   global->Set(v8::String::New("Delete"), v8::FunctionTemplate::New(JsDelete));

   JsCopy::Register(global);
   JsCompiler::Register(global);
   JsLinker::Register(global);
   JsResourceCompiler::Register(global);
   JsExe::Register(global);
   JsLibrarian::Register(global);
   JsLib::Register(global);
   JsFileToCpp::Register(global);

   SetArgs(global, args);

   context = v8::Context::New(NULL, global);
}

JavaScript::~JavaScript ()
{
   context.Dispose();
   while(!v8::V8::IdleNotification());  // What a crappy API... This collects all garbage, that's still laying around.
}

void JavaScript::SetArgs (v8::Handle<v8::ObjectTemplate>& global, const std::vector<std::string>& args)
{
   v8::Handle<v8::ObjectTemplate> obj = v8::ObjectTemplate::New();

   std::for_each(args.begin(), args.end(), [&obj] (const std::string& s) {
      size_t pos = s.find(':');
      if (pos == s.npos) pos = s.find('=');
      if (pos == s.npos) obj->Set(v8::String::New(s.c_str()), v8::String::New(""));
      else {
         std::string key = s.substr(0, pos);
         std::string val = s.substr(pos + 1);
         obj->Set(v8::String::New(key.c_str()), v8::String::New(val.c_str()));
      }
   });

   global->Set(v8::String::New("args"), obj);
}

void JavaScript::CheckException (v8::TryCatch& tryCatch)
{
   if (!tryCatch.HasCaught()) return;

   v8::HandleScope scope;

   std::string error = JavaScriptHelper::AsString(tryCatch.Exception());

   v8::Handle<v8::Message> message = tryCatch.Message();
   if (!message.IsEmpty()) {
      std::string filename = JavaScriptHelper::AsString(message->GetScriptResourceName());
      int line = message->GetLineNumber();
      
      std::stringstream ss;
      ss << filename << ": " << line << ": " << error << "\n";
      ss << JavaScriptHelper::AsString(message->GetSourceLine()) << "\n";

      int start = message->GetStartColumn();
      for (int i = 0; i < start; ++i) ss << ' ';

      int end = message->GetEndColumn();
      for (int i = start; i < end; ++i) ss << "^";

      ss << "\n";

      throw std::runtime_error(ss.str());
   }
   else {
      throw std::runtime_error(error);
   }
}

void JavaScript::Exec (const char* code, size_t len, const std::string& name)
{
   v8::HandleScope scope;

   v8::TryCatch tryCatch;

   v8::Handle<v8::String> source = v8::String::New(code, len);
   CheckException(tryCatch);

   v8::Handle<v8::String> origin = v8::String::New(name.c_str(), name.size());
   CheckException(tryCatch);

   v8::Handle<v8::Script> script = v8::Script::Compile(source, origin);
   CheckException(tryCatch);

   v8::Handle<v8::Value> result = script->Run();
   CheckException(tryCatch);
}

void JavaScript::ExecuteBuffer (const char* code, size_t len, const std::string& name)
{
   v8::Context::Scope contextScope(context);
   Exec(code, len, name);
}

void JavaScript::ExecuteString (const std::string& code, const std::string& name)
{
   ExecuteBuffer(code.c_str(), code.size(), name);
}

void JavaScript::ExecuteFile (const std::string& script)
{
   v8::HandleScope scope;

   if (!boost::filesystem::exists(script)) throw std::runtime_error("File " + script + " not found");

   boost::filesystem::path file = boost::filesystem::canonical(script);
   file.make_preferred();

   size_t size = static_cast<size_t>(boost::filesystem::file_size(file));

   boost::interprocess::file_mapping mapping(file.string().c_str(), boost::interprocess::read_only);
   boost::interprocess::mapped_region region(mapping, boost::interprocess::read_only, 0, size);

   ExecuteBuffer(static_cast<const char*>(region.get_address()), size, file.filename().string());
}

v8::Handle<v8::Value> JavaScript::JsExecuteString (const v8::Arguments& args)
{
   v8::HandleScope scope;

   std::string code = JavaScriptHelper::AsString(args[0]);
   if (code.empty()) return v8::ThrowException(v8::String::New("Expected String for ExecuteString()"));
   std::string name = JavaScriptHelper::AsString(args[1]);
   if (name.empty()) name = "<ExecuteString>";

   Exec(code.c_str(), code.size(), name);

   return v8::Undefined();
}

v8::Handle<v8::Value> JavaScript::JsExecuteFile (const v8::Arguments& args)
{
   v8::HandleScope scope;

   std::string script = JavaScriptHelper::AsString(args[0]);
   if (script.empty()) return v8::ThrowException(v8::String::New("Expected filename for ExecuteFile()"));
   if (!boost::filesystem::exists(script)) return v8::ThrowException(v8::String::New(std::string("File " + script + " not found").c_str()));

   boost::filesystem::path file = boost::filesystem::canonical(script);
   file.make_preferred();

   size_t size = static_cast<size_t>(boost::filesystem::file_size(file));

   boost::interprocess::file_mapping mapping(file.string().c_str(), boost::interprocess::read_only);
   boost::interprocess::mapped_region region(mapping, boost::interprocess::read_only, 0, size);

   Exec(static_cast<const char*>(region.get_address()), size, file.filename().string());

   return v8::Undefined();
}

v8::Handle<v8::Value> JavaScript::JsPrint (const v8::Arguments& args)
{
   std::cout << JavaScriptHelper::AsString(args) << std::endl;
   return v8::Undefined();
}

v8::Handle<v8::Value> JavaScript::JsQuit (const v8::Arguments& args)
{
   int exit_code = args[0]->Int32Value();
   exit(exit_code);
}

v8::Handle<v8::Value> JavaScript::JsRun (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;

   if (args.Length() < 1) return v8::ThrowException(v8::String::New("Expected commandline for Run()"));
   std::string command = JavaScriptHelper::AsString(args[0]);

   bool catchOutput = false;
   if (args.Length() > 1) catchOutput = args[1]->BooleanValue();

   auto tmpfile = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();


   if (catchOutput) {
      command += " 1>" + tmpfile.string() + " 2>&1";
   }

   int rc = std::system(command.c_str());
   if (rc) return v8::ThrowException(v8::String::New(std::string("Error running command " + command).c_str()));

   if (catchOutput) {
      {
         std::ifstream in(tmpfile.string());
         std::stringstream ss;
         ss << in.rdbuf();

         std::string r = ss.str();
         boost::trim_right(r);

         result = v8::String::New(r.c_str());
      }
      boost::filesystem::remove(tmpfile);
   }

   return scope.Close(result);
}

v8::Handle<v8::Value> JavaScript::JsSystem (const v8::Arguments& args)
{
   v8::HandleScope scope;

   if (args.Length() < 1) return v8::ThrowException(v8::String::New("Expected commandline for System()"));
   std::string command = JavaScriptHelper::AsString(args[0]);

   int rc = std::system(command.c_str());

   v8::Handle<v8::Integer> ret = v8::Integer::New(rc);
   return scope.Close(ret);
}

v8::Handle<v8::Value> JavaScript::JsGlob (const v8::Arguments& args)
{
   v8::HandleScope scope;

   std::string path = ".";
   std::string pattern = "*";

   if (args.Length() == 1) {
      pattern = JavaScriptHelper::AsString(args[0]);
   }
   else if (args.Length() == 2) {
      path = JavaScriptHelper::AsString(args[0]);
      pattern = JavaScriptHelper::AsString(args[1]);
   }
   else {
      return v8::ThrowException(v8::String::New("Expected one or two arguments for Glob()"));
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

   v8::Handle<v8::Array> result = v8::Array::New(files.size());
   for (size_t i = 0; i < files.size(); ++i) result->Set(i, v8::String::New(files[i].c_str()));
   return scope.Close(result);
}

v8::Handle<v8::Value> JavaScript::JsFullPath (const v8::Arguments& args)
{
   v8::HandleScope scope;

   std::string path = ".";
   if (args.Length()) path = JavaScriptHelper::AsString(args[0]);

   auto full = boost::filesystem::canonical(path);
   full.make_preferred();

   v8::Local<v8::String> result = v8::String::New(full.string().c_str());
   return scope.Close(result);
}

v8::Handle<v8::Value> JavaScript::JsBuild (const v8::Arguments& args)
{
   v8::HandleScope scope;

   if (args.Length() != 1) return v8::ThrowException(v8::String::New("Expected one argument for Build()"));

   auto current = boost::filesystem::current_path();
   boost::filesystem::current_path(JavaScriptHelper::AsString(args[0]));

   try {
      boost::filesystem::path file = boost::filesystem::canonical("FBuild.js");
      file.make_preferred();

      size_t size = static_cast<size_t>(boost::filesystem::file_size(file));

      boost::interprocess::file_mapping mapping(file.string().c_str(), boost::interprocess::read_only);
      boost::interprocess::mapped_region region(mapping, boost::interprocess::read_only, 0, size);

      Exec(static_cast<const char*>(region.get_address()), size, file.filename().string());
   }
   catch (std::exception& e) {
      boost::filesystem::current_path(current);
      return v8::ThrowException(v8::String::New(e.what()));
   }

   boost::filesystem::current_path(current);

   return v8::Undefined();
}

v8::Handle<v8::Value> JavaScript::JsFileOutOfDate (const v8::Arguments& args)
{
   v8::HandleScope scope;

   if (args.Length() < 2) return v8::ThrowException(v8::String::New("Expected two or more arguments for FileOutOfDate()"));

   FileOutOfDate outOfDate;
   outOfDate.Parent(JavaScriptHelper::AsString(args[0]));

   for (int i = 1; i < args.Length(); ++i) {
      if (args[i]->IsArray()) {
         v8::Local<v8::Object> arr = args[i]->ToObject();
         size_t length = arr->Get(v8::String::New("length"))->Int32Value();
         for (size_t i = 0; i < length; ++i) outOfDate.AddFile(JavaScriptHelper::AsString(arr->Get(i)));
      }
      else {
         outOfDate.AddFile(JavaScriptHelper::AsString(args[i]));
      }
   }

   v8::Handle<v8::Value> result = v8::Boolean::New(outOfDate.Go());
   return scope.Close(result);
}

v8::Handle<v8::Value> JavaScript::JsChangeDirectory (const v8::Arguments& args)
{
   v8::HandleScope scope;

   if (args.Length() != 1) return v8::ThrowException(v8::String::New("Expected one argument for ChangeDirectory()"));

   boost::filesystem::current_path(JavaScriptHelper::AsString(args[0]));

   return v8::Undefined();
}

v8::Handle<v8::Value> JavaScript::JsStringToFile (const v8::Arguments& args)
{
   v8::HandleScope scope;

   if (args.Length() != 2) return v8::ThrowException(v8::String::New("Expected two arguments for StringToFile()"));

   std::ofstream out(JavaScriptHelper::AsString(args[0]), std::ofstream::trunc);
   if (out.fail()) return v8::ThrowException(v8::String::New(std::string("Error opening " + JavaScriptHelper::AsString(args[0])).c_str()));

   out << JavaScriptHelper::AsString(args[1]);

   return v8::Undefined();
}

v8::Handle<v8::Value> JavaScript::JsGetEnv (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;

   if (args.Length() != 1) return v8::ThrowException(v8::String::New("Expected one argument for GetEnv()"));

   const char* env = std::getenv(JavaScriptHelper::AsString(args[0]).c_str());
   if (env) result = v8::String::New(env);

   return scope.Close(result);
}

v8::Handle<v8::Value> JavaScript::JsSetEnv (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;

   if (args.Length() != 2) return v8::ThrowException(v8::String::New("Expected two arguments for SetEnv()"));

   std::string arg = JavaScriptHelper::AsString(args[0]) + "=" + JavaScriptHelper::AsString(args[1]);

   int rc = _putenv(arg.c_str());
   if (rc) return v8::ThrowException(v8::String::New(std::string("Error putting environment " + JavaScriptHelper::AsString(args[0])).c_str()));

   return v8::Undefined();
}

v8::Handle<v8::Value> JavaScript::JsTouch (const v8::Arguments& args)
{
   v8::HandleScope scope;

   if (args.Length() < 1) return v8::ThrowException(v8::String::New("Expected filename(s) for Touch()"));
   std::vector<std::string> files = JavaScriptHelper::AsStringVector(args);

   try {
      std::for_each(files.begin(), files.end(), [] (const std::string& filename) {
         if (!boost::filesystem::exists(filename)) throw std::exception(std::string("File " + filename + " does not exist").c_str());
         if (!boost::filesystem::is_regular_file(filename)) throw std::exception(std::string(filename + " is not a file").c_str());
         boost::filesystem::last_write_time(filename, std::time(nullptr));
      });
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return v8::Undefined();
}

v8::Handle<v8::Value> JavaScript::JsDelete (const v8::Arguments& args)
{
   v8::HandleScope scope;

   if (args.Length() < 1) return v8::ThrowException(v8::String::New("Expected filename(s) for Delete()"));
   std::vector<std::string> files = JavaScriptHelper::AsStringVector(args);

   try {
      std::for_each(files.begin(), files.end(), [] (const std::string& filename) {
         boost::system::error_code rc;
         boost::filesystem::remove_all(filename, rc);
      });
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return v8::Undefined();
}
