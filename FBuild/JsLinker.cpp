/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsLinker.h"

v8::Handle<v8::Value> JsLinker::GetSet (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;

   const std::string funcname = AsString(args.Callee()->GetName());

   try {
      JsLinker* self = Unwrap<JsLinker>(args);

      if (args.Length() == 0) {
         if (funcname == "Build") result = Value(self->linker.Build()); 
         else if (funcname == "Output") result = Value(self->linker.Output());
         else if (funcname == "ImportLib") result = Value(self->linker.ImportLib());
         else if (funcname == "Files") result = Value(self->linker.Files());
         else if (funcname == "LibPath") result = Value(self->linker.Libpath());
         else if (funcname == "Libs") result = Value(self->linker.Libs());
         else if (funcname == "Def") result = Value(self->linker.Def());
         else if (funcname == "DependencyCheck") result = Value(self->linker.DependencyCheck());
         else if (funcname == "Args") result = Value(self->linker.Args());
         else if (funcname == "BeforeLink") result = Value(self->linker.BeforeLink());
      }
      else {
         if (funcname == "Build") self->linker.Build(AsString(args[0]));
         else if (funcname == "Output") self->linker.Output(AsString(args[0]));
         else if (funcname == "ImportLib") self->linker.ImportLib(AsString(args[0]));
         else if (funcname == "Files") self->linker.Files(AsStringVector(args));
         else if (funcname == "LibPath") self->linker.Libpath(AsStringVector(args));
         else if (funcname == "Libs") self->linker.Libs(AsStringVector(args));
         else if (funcname == "Def") self->linker.Def(AsString(args[0]));
         else if (funcname == "DependencyCheck") self->linker.DependencyCheck(AsBool(args[0]));
         else if (funcname == "Args") self->linker.Args(AsString(args));
         else if (funcname == "BeforeLink") self->linker.BeforeLink(AsCallback(args[0]));

         result = args.This();
      }
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}

v8::Handle<v8::Value> JsLinker::Link (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;
   JsLinker* self = Unwrap<JsLinker>(args);

   try {
      self->linker.Link();
      result = args.This();
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}

void JsLinker::Register (v8::Handle<v8::ObjectTemplate>& global)
{
   v8::HandleScope scope;

   v8::Handle<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(JavaScriptHelper::Construct<JsLinker>);
   funcTemplate->SetClassName(v8::String::New("Linker"));

   v8::Handle<v8::ObjectTemplate> proto = funcTemplate->PrototypeTemplate();
   proto->Set("Build", v8::FunctionTemplate::New(GetSet));
   proto->Set("Output", v8::FunctionTemplate::New(GetSet));
   proto->Set("ImportLib", v8::FunctionTemplate::New(GetSet));
   proto->Set("Def", v8::FunctionTemplate::New(GetSet));
   proto->Set("LibPath", v8::FunctionTemplate::New(GetSet));
   proto->Set("Libs", v8::FunctionTemplate::New(GetSet));
   proto->Set("Files", v8::FunctionTemplate::New(GetSet));
   proto->Set("DependencyCheck", v8::FunctionTemplate::New(GetSet));
   proto->Set("Args", v8::FunctionTemplate::New(GetSet));
   proto->Set("BeforeLink", v8::FunctionTemplate::New(GetSet));

   proto->Set("Link", v8::FunctionTemplate::New(Link));

   v8::Handle<v8::ObjectTemplate> inst = funcTemplate->InstanceTemplate();
   inst->SetInternalFieldCount(1);

   global->Set(v8::String::New("Linker"), funcTemplate);
}
