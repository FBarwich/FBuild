#include "JsFileToCpp.h"



v8::Handle<v8::Value> JsFileToCpp::GetSet (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;

   const std::string funcname = AsString(args.Callee()->GetName());

   try {
      JsFileToCpp* self = Unwrap<JsFileToCpp>(args);

      if (args.Length() == 0) {
         if (funcname == "DependencyCheck") result = Value(self->file2cpp.DependencyCheck());
         else if (funcname == "Const") result = Value(self->file2cpp.Const());
         else if (funcname == "NullTerminator") result = Value(self->file2cpp.NullTerminator());
         else if (funcname == "Infile") result = Value(self->file2cpp.NullTerminator());
         else if (funcname == "Outfile") result = Value(self->file2cpp.Outfile());
         else if (funcname == "Namespace") result = Value(self->file2cpp.Namespace());
         else if (funcname == "Array") result = Value(self->file2cpp.Array());
         else if (funcname == "Ptr") result = Value(self->file2cpp.Ptr());
         else if (funcname == "Intro") result = Value(self->file2cpp.Intro());
         else if (funcname == "Outro") result = Value(self->file2cpp.Outro());
         else if (funcname == "Additional") result = Value(self->file2cpp.Additional());
      }
      else {
         if (funcname == "DependencyCheck") self->file2cpp.DependencyCheck(AsBool(args[0]));
         else if (funcname == "Const") self->file2cpp.Const(AsBool(args[0]));
         else if (funcname == "NullTerminator") self->file2cpp.NullTerminator(AsBool(args[0]));
         else if (funcname == "Infile") self->file2cpp.Infile(AsString(args[0]));
         else if (funcname == "Outfile") self->file2cpp.Outfile(AsString(args[0]));
         else if (funcname == "Namespace") self->file2cpp.Namespace(AsString(args[0]));
         else if (funcname == "Array") self->file2cpp.Array(AsString(args[0]));
         else if (funcname == "Ptr") self->file2cpp.Ptr(AsString(args[0]));
         else if (funcname == "Intro") self->file2cpp.Intro(AsString(args[0]));
         else if (funcname == "Outro") self->file2cpp.Outro(AsString(args[0]));
         else if (funcname == "Additional") self->file2cpp.Additional(AsString(args[0]));

         result = args.This();
      }
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}


v8::Handle<v8::Value> JsFileToCpp::Create (const v8::Arguments& args)
{
   v8::HandleScope scope;
   v8::Handle<v8::Value> result;
   JsFileToCpp* self = Unwrap<JsFileToCpp>(args);

   try {
      self->file2cpp.Create();
      result = args.This();
   }
   catch (std::exception& e) {
      return v8::ThrowException(v8::String::New(e.what()));
   }

   return scope.Close(result);
}

void JsFileToCpp::Register (v8::Handle<v8::ObjectTemplate>& global)
{
   v8::HandleScope scope;

   v8::Handle<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(JavaScriptHelper::Construct<JsFileToCpp>);
   funcTemplate->SetClassName(v8::String::New("FileToCpp"));

   v8::Handle<v8::ObjectTemplate> proto = funcTemplate->PrototypeTemplate();
   proto->Set("DependencyCheck", v8::FunctionTemplate::New(GetSet));
   proto->Set("Const", v8::FunctionTemplate::New(GetSet));
   proto->Set("NullTerminator", v8::FunctionTemplate::New(GetSet));
   proto->Set("Infile", v8::FunctionTemplate::New(GetSet));
   proto->Set("Outfile", v8::FunctionTemplate::New(GetSet));
   proto->Set("Namespace", v8::FunctionTemplate::New(GetSet));
   proto->Set("Array", v8::FunctionTemplate::New(GetSet));
   proto->Set("Ptr", v8::FunctionTemplate::New(GetSet));
   proto->Set("Intro", v8::FunctionTemplate::New(GetSet));
   proto->Set("Outro", v8::FunctionTemplate::New(GetSet));
   proto->Set("Additional", v8::FunctionTemplate::New(GetSet));

   proto->Set("Create", v8::FunctionTemplate::New(Create));

   v8::Handle<v8::ObjectTemplate> inst = funcTemplate->InstanceTemplate();
   inst->SetInternalFieldCount(1);

   global->Set(v8::String::New("FileToCpp"), funcTemplate);
}

