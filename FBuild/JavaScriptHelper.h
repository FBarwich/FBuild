/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include "../Duktape/duktape.h"

#include <boost/utility/string_ref.hpp> 

#include <boost/lexical_cast.hpp>


namespace JavaScriptHelper {

   inline void Throw(duk_context* duktapeContext, boost::string_ref what)
   {
      duk_push_error_object(duktapeContext, DUK_ERR_ERROR, what.data());
      duk_throw(duktapeContext);
   }

   inline std::string AsString(duk_context* duktapeContext, int args = -1)
   {
      std::string result;

      if (args == -1) args = duk_get_top(duktapeContext);

      for (int i = 0; i < args; ++i) {
         if (result.size()) result += ' ';
         result += duk_to_string(duktapeContext, i);
      }

      return result;
   }

   inline std::vector<std::string> AsStringVector(duk_context* duktapeContext, int args = -1)
   {
      std::vector<std::string> result;

      if (args == -1) args = duk_get_top(duktapeContext);

      for (int i = 0; i < args; ++i) {
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

   inline std::vector<int> AsIntVector(duk_context* duktapeContext, int args = -1)
   {
      std::vector<int> result;

      if (args == -1) args = duk_get_top(duktapeContext);

      for (int i = 0; i < args; ++i) {
         if (duk_is_array(duktapeContext, i)) {
            duk_enum(duktapeContext, i, DUK_ENUM_ARRAY_INDICES_ONLY);

            while (duk_next(duktapeContext, -1, 1)) {
               result.push_back(duk_to_int(duktapeContext, -1));
               duk_pop_2(duktapeContext);
            }

            duk_pop(duktapeContext);
         }
         else {
            result.push_back(duk_to_int(duktapeContext, i));
         }
      }

      return result;
   }

   inline void PushArray(duk_context* duktapeContext, const std::vector<std::string>& array)
   {
      duk_push_array(duktapeContext);

      for (size_t i = 0; i < array.size(); ++i) {
         duk_push_string(duktapeContext, array[i].c_str());
         duk_put_prop_index(duktapeContext, -2, i);
      }
   }

   inline void PushArray(duk_context* duktapeContext, const std::vector<int>& array)
   {
      duk_push_array(duktapeContext);

      for (size_t i = 0; i < array.size(); ++i) {
         duk_push_int(duktapeContext, array[i]);
         duk_put_prop_index(duktapeContext, -2, i);
      }
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

   inline void StashCallback(duk_context* duktapeContext, int index, const std::string& key, void* obj)
   {
      if (!duk_is_function(duktapeContext, index)) JavaScriptHelper::Throw(duktapeContext, "Function expected");

      std::string k = key + boost::lexical_cast<std::string>((size_t)obj);
      duk_push_global_stash(duktapeContext);
      duk_dup(duktapeContext, index);
      duk_put_prop_string(duktapeContext, -2, k.c_str());
      duk_pop(duktapeContext);
   }

   inline void PushStashedCallback(duk_context* duktapeContext, const std::string& key, void* obj)
   {
      std::string k = key + boost::lexical_cast<std::string>((size_t)obj);
      duk_push_global_stash(duktapeContext);
      duk_get_prop_string(duktapeContext, -1, k.c_str());
   }

   inline void CallStashedCallback(duk_context* duktapeContext, const std::string& key, void* obj)
   {
      int top = duk_get_top(duktapeContext);

      std::string k = key + boost::lexical_cast<std::string>((size_t)obj);
      duk_push_global_stash(duktapeContext);
      duk_get_prop_string(duktapeContext, -1, k.c_str());
      duk_call(duktapeContext, 0);

      duk_set_top(duktapeContext, top);
   }
}
