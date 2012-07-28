/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JsCopy.h"


v8::Handle<v8::Value> JsCopy::CopyFunc (const v8::Arguments& args)
{
   v8::HandleScope scope;

   if (args.Length() < 2) return v8::ThrowException(v8::String::New("Expected two or three arguments for Copy()"));

   Copy copy;
   copy.Source(AsString(args[0]));
   copy.Dest(AsString(args[1]));
   if (args.Length() > 2) copy.DependencyCheck(AsBool(args[2]));

   int copied = copy.Go();

   v8::Handle<v8::Integer> ret = v8::Integer::New(copied);
   return scope.Close(ret);
}

v8::Handle<v8::Value> JsCopy::Construct (const v8::Arguments& args)
{
   if (!args.IsConstructCall()) return v8::ThrowException(v8::String::New("Wrong use of constructor (CopyObj)"));

   v8::HandleScope scope;

   v8::Persistent<v8::Object> self = v8::Persistent<v8::Object>::New(args.This());


   JsCopy* me = new JsCopy;
   if (args.Length() > 1) {
      me->copy.Source(AsString(args[0]));
      me->copy.Dest(AsString(args[1]));
   }
   if (args.Length() > 2) {
      me->copy.DependencyCheck(AsBool(args[2]));
   }

   self.MakeWeak(me, WeakCallback<JsCopy>);
   self->SetInternalField(0, v8::External::New(me));

   return v8::Undefined();
}

v8::Handle<v8::Value> JsCopy::GetSet (const v8::Arguments& args)
{
   v8::HandleScope scope;

   v8::Handle<v8::Value> result;

   std::string funcname = AsString(args.Callee()->GetName());
   JsCopy* self = Unwrap<JsCopy>(args);

   if (args.Length() == 0) {
      if (funcname == "Source") result = Value(self->copy.Source());
      else if (funcname == "Dest") result = Value(self->copy.Dest());
      else if (funcname == "DependencyCheck") result = Value(self->copy.DependencyCheck());
   }
   else {
      if (funcname == "Source") self->copy.Source(AsString(args[0]));
      else if (funcname == "Dest") self->copy.Dest(AsString(args[0]));
      else if (funcname == "DependencyCheck") self->copy.DependencyCheck(AsBool(args[0]));
   }

   return scope.Close(result);
}

v8::Handle<v8::Value> JsCopy::NeedsCopy (const v8::Arguments& args)
{
   v8::HandleScope scope;
   JsCopy* self = Unwrap<JsCopy>(args);

   v8::Local<v8::Value> result = v8::BooleanObject::New(self->copy.NeedsCopy());
   return scope.Close(result);
}

v8::Handle<v8::Value> JsCopy::Go (const v8::Arguments& args)
{
   v8::HandleScope scope;

   JsCopy* self = Unwrap<JsCopy>(args);

   v8::Local<v8::Value> result = v8::Integer::New(self->copy.Go());
   return scope.Close(result);
}

void JsCopy::Register (v8::Handle<v8::ObjectTemplate>& global)
{
   v8::HandleScope scope;

   global->Set(v8::String::New("Copy"), v8::FunctionTemplate::New(CopyFunc));



   v8::Handle<v8::FunctionTemplate> funcTemplate = v8::FunctionTemplate::New(Construct);
   funcTemplate->SetClassName(v8::String::New("CopyObj"));

   v8::Handle<v8::ObjectTemplate> proto = funcTemplate->PrototypeTemplate();
   proto->Set("NeedsCopy", v8::FunctionTemplate::New(NeedsCopy));
   proto->Set("Copy", v8::FunctionTemplate::New(Go));

   proto->Set("Source", v8::FunctionTemplate::New(GetSet));
   proto->Set("Dest", v8::FunctionTemplate::New(GetSet));
   proto->Set("DependencyCheck", v8::FunctionTemplate::New(GetSet));

   v8::Handle<v8::ObjectTemplate> inst = funcTemplate->InstanceTemplate();
   inst->SetInternalFieldCount(1);

   global->Set(v8::String::New("CopyObj"), funcTemplate);
}
