/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsLibrarian.h"


v8::Handle<v8::Value> JsLibrarian::GetSet (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;

   JsLibrarian* self = Unwrap<JsLibrarian>(args);
   const std::string funcname = AsString(args.Callee()->GetName());

   try {
      if (args.Length() == 0) {
         if (funcname == "Output") result = Value(self->librarian.Output());
         else if (funcname == "Files") result = Value(self->librarian.Files());
         else if (funcname == "DependencyCheck") result = Value(self->librarian.DependencyCheck());
         else if (funcname == "BeforeLink") result = Value(self->librarian.BeforeLink());
      }
      else {
         if (funcname == "Output") self->librarian.Output(AsString(args[0]));
         else if (funcname == "Files") self->librarian.Files(AsStringVector(args));
         else if (funcname == "DependencyCheck") self->librarian.DependencyCheck(AsBool(args[0]));
         else if (funcname == "BeforeLink") self->librarian.BeforeLink(AsCallback(args[0]));

         result = args.This();
      }
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}


v8::Handle<v8::Value> JsLibrarian::Create (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;
   JsLibrarian* self = Unwrap<JsLibrarian>(args);

   try {
      self->librarian.Create();
      result = args.This();
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}

void JsLibrarian::Register (v8::Handle<v8::ObjectTemplate>& global)
{
   v8::HandleScope scope;

   v8::Handle<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(JavaScriptHelper::Construct<JsLibrarian>);
   funcTemplate->SetClassName(v8::String::New("Librarian"));

   v8::Handle<v8::ObjectTemplate> proto = funcTemplate->PrototypeTemplate();
   proto->Set("Output", v8::FunctionTemplate::New(GetSet));
   proto->Set("Files", v8::FunctionTemplate::New(GetSet));
   proto->Set("DependencyCheck", v8::FunctionTemplate::New(GetSet));
   proto->Set("BeforeLink", v8::FunctionTemplate::New(GetSet));

   proto->Set("Create", v8::FunctionTemplate::New(Create));


   v8::Handle<v8::ObjectTemplate> inst = funcTemplate->InstanceTemplate();
   inst->SetInternalFieldCount(1);

   global->Set(v8::String::New("Librarian"), funcTemplate);
}
