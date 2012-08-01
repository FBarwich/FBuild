/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsResourceCompiler.h"


v8::Handle<v8::Value> JsResourceCompiler::GetSet (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;

   JsResourceCompiler* self = Unwrap<JsResourceCompiler>(args);
   const std::string funcname = AsString(args.Callee()->GetName());

   try {

      if (args.Length() == 0) {
         if (funcname == "Files") result = Value(self->resourceCompiler.Files());
         else if (funcname == "Outdir") result = Value(self->resourceCompiler.Outdir());
         else if (funcname == "DependencyCheck") result = Value(self->resourceCompiler.DependencyCheck());
         else if (funcname == "Outfiles") result = Value(self->resourceCompiler.Outfiles());
      }
      else {
         if (funcname == "Files") self->resourceCompiler.Files(AsStringVector(args));
         else if (funcname == "Outdir") self->resourceCompiler.Outdir(AsString(args[0]));
         else if (funcname == "DependencyCheck") self->resourceCompiler.DependencyCheck(AsBool(args[0]));

         result = args.This();
      }
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}


v8::Handle<v8::Value> JsResourceCompiler::Compile (const v8::Arguments& args)
{
   v8::HandleScope scope;
   JsResourceCompiler* self = Unwrap<JsResourceCompiler>(args);

   try {
      self->resourceCompiler.Compile();
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   v8::Handle<v8::Value> result = args.This();
   return scope.Close(result);
}

void JsResourceCompiler::Register (v8::Handle<v8::ObjectTemplate>& global)
{
   v8::HandleScope scope;

   v8::Handle<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(JavaScriptHelper::Construct<JsResourceCompiler>);
   funcTemplate->SetClassName(v8::String::New("ResourceCompiler"));

   v8::Handle<v8::ObjectTemplate> proto = funcTemplate->PrototypeTemplate();
   proto->Set("Files", v8::FunctionTemplate::New(GetSet));
   proto->Set("Outdir", v8::FunctionTemplate::New(GetSet));
   proto->Set("DependencyCheck", v8::FunctionTemplate::New(GetSet));
   proto->Set("Outfiles", v8::FunctionTemplate::New(GetSet));

   proto->Set("Compile", v8::FunctionTemplate::New(Compile));


   v8::Handle<v8::ObjectTemplate> inst = funcTemplate->InstanceTemplate();
   inst->SetInternalFieldCount(1);

   global->Set(v8::String::New("ResourceCompiler"), funcTemplate);
}

