/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include "../Duktape/duktape.h"

#include <boost/utility/string_ref.hpp> 


namespace JavaScriptHelper {

   inline void Throw(duk_context* duktapeContext, boost::string_ref what)
   {
      duk_push_string(duktapeContext, what.data());
      duk_throw(duktapeContext);
   }

   inline std::vector<std::string> AsStringVector(duk_context* duktapeContext)
   {
      std::vector<std::string> result;

      int top = duk_get_top(duktapeContext);

      for (int i = 0; i < top; ++i) {
         if (duk_is_array(duktapeContext, i)) {
            duk_enum(duktapeContext, i, DUK_ENUM_ARRAY_INDICES_ONLY);

            while (duk_next(duktapeContext, -1, 1)) {
               result.push_back(duk_to_string(duktapeContext, -1));
               duk_pop_2(duktapeContext);
            }

            duk_pop(duktapeContext);
         }
         else {
            result.push_back(duk_to_string(duktapeContext, i));
         }
      }

      return result;
   }

   template<typename T>
   T* CppObject(duk_context* duktapeContext)
   {
      if (!duk_is_object(duktapeContext, -1)) throw std::exception("Internal Error: expected object on top of the stack");
      duk_get_prop_string(duktapeContext, -1, "__Ptr");
      if (!duk_is_pointer(duktapeContext, -1)) throw std::exception("Internal Error: expected property '__Ptr' to hold the C++ Object");
      void* ptr = duk_get_pointer(duktapeContext, -1);
      duk_pop(duktapeContext);
      if (!ptr) throw std::exception("Internal Error: Nullpointer! C++ Object ist Null");
      return static_cast<T*>(ptr);
   }

}


/* TODO

class JavaScriptHelper {
public:
   static int AsInt (const v8::Handle<v8::Value>& val) { return val->Int32Value(); }
   static bool AsBool (const v8::Handle<v8::Value>& val) { return val->BooleanValue(); }

   static std::string AsString (const v8::Arguments& args)
   {
      std::string result;

      for (int i = 0; i < args.Length(); ++i) {
         if (!result.empty()) result += ' ';
         result += AsString(args[i]);
      }

      return result;
   }

   struct FunctorWrapper {
      v8::HandleScope scope; // scope must be created before the local!
      v8::Local<v8::Function> function;

      FunctorWrapper (v8::Local<v8::Function> f) 
         : function(v8::Local<v8::Function>::New(f))
      {
      }

      FunctorWrapper (const FunctorWrapper& other) 
         : function(v8::Local<v8::Function>::New(other.function))
      {
      }

      void operator() () 
      {
         function->Call(v8::Context::GetCurrent()->Global(), 0, nullptr);
      }
   };

   static std::function<void()> AsCallback (const v8::Handle<v8::Value>& val) 
   {
      return FunctorWrapper(v8::Local<v8::Function>::Cast(val->ToObject()));
   }


   static std::vector<int> AsIntVector (const v8::Arguments& args)
   {
      std::vector<int> result;

      for (int i = 0; i < args.Length(); ++i) {
         if (args[i]->IsArray()) {
            v8::Local<v8::Object> arr = args[i]->ToObject();
            size_t length = arr->Get(v8::String::New("length"))->Int32Value();
            for (size_t i = 0; i < length; ++i) result.push_back(AsInt(arr->Get(i)));
         }
         else {
            result.push_back(AsInt(args[i]));
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
*/