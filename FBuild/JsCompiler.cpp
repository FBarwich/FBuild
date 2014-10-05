/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsCompiler.h"


/* TODO


v8::Handle<v8::Value> JsCompiler::GetSet (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;

   try {
      const std::string funcname = AsString(args.Callee()->GetName());
      JsCompiler* self = Unwrap<JsCompiler>(args);

      if (args.Length() == 0) {
         if (funcname == "Build") result = Value(self->compiler.Build()); 
         else if (funcname == "ObjDir") result = Value(self->compiler.ObjDir());
         else if (funcname == "Includes") result = Value(self->compiler.Includes());
         else if (funcname == "Defines") result = Value(self->compiler.Defines());
         else if (funcname == "CRT") result = Value(self->compiler.CRT());
         else if (funcname == "Threads") result = Value(self->compiler.Threads());
         else if (funcname == "Args") result = Value(self->compiler.Args());
         else if (funcname == "PrecompiledHeader") result = Value(self->compiler.PrecompiledHeader());
         else if (funcname == "DependencyCheck") result = Value(self->compiler.DependencyCheck());
         else if (funcname == "Files") result = Value(self->compiler.Files());
         else if (funcname == "WarningLevel") result = Value(self->compiler.WarnLevel());
         else if (funcname == "WarningAsError") result = Value(self->compiler.WarningAsError());
         else if (funcname == "WarningDisable") result = Value(self->compiler.WarningDisable());
         else if (funcname == "BeforeCompile") result = Value(self->compiler.BeforeCompile());
      }
      else {
         if (funcname == "Build") self->compiler.Build(AsString(args[0]));
         else if (funcname == "ObjDir") self->compiler.ObjDir(AsString(args[0]));
         else if (funcname == "Includes") self->compiler.Includes(AsStringVector(args));
         else if (funcname == "Defines") self->compiler.Defines(AsStringVector(args));
         else if (funcname == "CRT") self->compiler.CRT(AsString(args[0]));
         else if (funcname == "Threads") self->compiler.Threads(AsInt(args[0]));
         else if (funcname == "Args") self->compiler.Args(AsString(args));
         else if (funcname == "PrecompiledHeader") self->compiler.PrecompiledHeader(AsString(args[0]), AsString(args[1]));
         else if (funcname == "DependencyCheck") self->compiler.DependencyCheck(AsBool(args[0]));
         else if (funcname == "Files") self->compiler.Files(AsStringVector(args));
         else if (funcname == "WarningLevel") self->compiler.WarnLevel(AsInt(args[0]));
         else if (funcname == "WarningAsError") self->compiler.WarningAsError(AsBool(args[0]));
         else if (funcname == "WarningDisable") self->compiler.WarningDisable(AsIntVector(args));
         else if (funcname == "BeforeCompile") self->compiler.BeforeCompile(AsCallback(args[0]));

         result = args.This();
      }
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}

v8::Handle<v8::Value> JsCompiler::ObjFiles (const v8::Arguments& args)
{
   v8::HandleScope scope;
   JsCompiler* self = Unwrap<JsCompiler>(args);

   v8::Handle<v8::Value> result;

   try {
      result = Value(self->compiler.ObjFiles());
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}

v8::Handle<v8::Value> JsCompiler::CompiledObjFiles (const v8::Arguments& args)
{
   v8::HandleScope scope;
   JsCompiler* self = Unwrap<JsCompiler>(args);

   v8::Handle<v8::Value> result;

   try {
      result = Value(self->compiler.CompiledObjFiles());
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}

v8::Handle<v8::Value> JsCompiler::Compile (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;
   JsCompiler* self = Unwrap<JsCompiler>(args);

   try {
      self->compiler.Compile();
      result = args.This();
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}

void JsCompiler::Register (v8::Handle<v8::ObjectTemplate>& global)
{
   v8::HandleScope scope;

   v8::Handle<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(JavaScriptHelper::Construct<JsCompiler>);
   funcTemplate->SetClassName(v8::String::New("Compiler"));

   v8::Handle<v8::ObjectTemplate> proto = funcTemplate->PrototypeTemplate();
   proto->Set("Build", v8::FunctionTemplate::New(GetSet));
   proto->Set("ObjDir", v8::FunctionTemplate::New(GetSet));
   proto->Set("Includes", v8::FunctionTemplate::New(GetSet));
   proto->Set("Defines", v8::FunctionTemplate::New(GetSet));
   proto->Set("CRT", v8::FunctionTemplate::New(GetSet));
   proto->Set("Threads", v8::FunctionTemplate::New(GetSet));
   proto->Set("Args", v8::FunctionTemplate::New(GetSet));
   proto->Set("PrecompiledHeader", v8::FunctionTemplate::New(GetSet));
   proto->Set("DependencyCheck", v8::FunctionTemplate::New(GetSet));
   proto->Set("Files", v8::FunctionTemplate::New(GetSet));
   proto->Set("WarningLevel", v8::FunctionTemplate::New(GetSet));
   proto->Set("WarningAsError", v8::FunctionTemplate::New(GetSet));
   proto->Set("WarningDisable", v8::FunctionTemplate::New(GetSet));
   proto->Set("BeforeCompile", v8::FunctionTemplate::New(GetSet));

   proto->Set("Compile", v8::FunctionTemplate::New(Compile));
   proto->Set("ObjFiles", v8::FunctionTemplate::New(ObjFiles));
   proto->Set("CompiledObjFiles", v8::FunctionTemplate::New(CompiledObjFiles));

   v8::Handle<v8::ObjectTemplate> inst = funcTemplate->InstanceTemplate();
   inst->SetInternalFieldCount(1);

   global->Set(v8::String::New("Compiler"), funcTemplate);
}


*/