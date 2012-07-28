/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <v8.h>

class JavaScriptHelper {
public:
   static int AsInt (const v8::Handle<v8::Value>& val) { return val->Int32Value(); }
   static bool AsBool (const v8::Handle<v8::Value>& val) { return val->BooleanValue(); }

   static std::string AsString (const v8::Handle<v8::Value>& val)
   {
      v8::String::AsciiValue ascii(val);
      if (!*ascii) return "";
      else return std::string(*ascii, ascii.length());
   }

   static std::string AsString (const v8::Arguments& args)
   {
      std::string result;

      for (size_t i = 0; i < args.Length(); ++i) {
         if (!result.empty()) result += ' ';
         result += AsString(args[i]);
      }

      return result;
   }

   static std::vector<std::string> AsStringVector (const v8::Arguments& args)
   {
      std::vector<std::string> result;

      for (size_t i = 0; i < args.Length(); ++i) {
         if (args[i]->IsArray()) {
            v8::Local<v8::Object> arr = args[i]->ToObject();
            size_t length = arr->Get(v8::String::New("length"))->Int32Value();
            for (size_t i = 0; i < length; ++i) result.push_back(AsString(arr->Get(i)));
         }
         else {
            result.push_back(AsString(args[i]));
         }
      }

      return result;
   }

   static v8::Handle<v8::Value> Value (int i) { return v8::Integer::New(i); }
   static v8::Handle<v8::Value> Value (bool b) { return v8::Boolean::New(b); }
   static v8::Handle<v8::Value> Value (const std::string& s) { return v8::String::New(s.c_str(), s.size()); }

   template<typename T> static v8::Handle<v8::Value> Value (const std::vector<T>& v)
   {
      v8::Local<v8::Array> arr = v8::Array::New(v.size());
      for (size_t i = 0; i < v.size(); ++i) arr->Set(i, Value(v[i]));
      return arr;
   }


   template<typename CLASS>
   static v8::Handle<v8::Value> Construct (const v8::Arguments& args)
   {
      if (!args.IsConstructCall()) return v8::ThrowException(v8::String::New("Illegal use of constructor"));

      v8::HandleScope scope;
      v8::Persistent<v8::Object> self = v8::Persistent<v8::Object>::New(args.This());

      CLASS* c = new CLASS;
      self.MakeWeak(c, WeakCallback<CLASS>);
      self->SetInternalField(0, v8::External::New(c));

      return v8::Undefined();
   }


   template<typename T> static inline T* Unwrap (const v8::Arguments& args)
   {
      v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(args.Holder()->GetInternalField(0));
      void* ptr = wrap->Value();
      return static_cast<T*>(ptr);
   }

   template<typename T> static void WeakCallback (v8::Persistent<v8::Value> object, void* ptr)
   {
      T* obj = static_cast<T*>(ptr);
      delete obj;
      object.Dispose();
   }

};