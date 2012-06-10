/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>


inline std::ostream& WriteToOstream (std::ostream& stream, const void* what, size_t size)
{
   stream.write(static_cast<const char*>(what), size);
   return stream;
}

inline std::istream& ReadFromIStream (std::istream& stream, void* what, size_t size)
{
   stream.read(static_cast<char*>(what), size);
   return stream;
}

inline std::ostream& WriteStringToOStream (std::ostream& stream, const char* str, size_t length)
{
   WriteToOstream(stream, &length, sizeof(length));
   if (length) stream.write(str, length);
   return stream;
}

inline std::ostream& WriteStringToOStream (std::ostream& stream, const char* str)
{
   std::string tmp(str);
   return WriteStringToOStream(stream, tmp.c_str(), tmp.size());
}

inline std::istream& ReadStringFromIStream (std::istream& stream, std::string& str)
{
   size_t length;
   ReadFromIStream(stream, &length, sizeof(length));

   if (length) {
      str.assign(length, '\0');
      stream.read(&str[0], length);
   } 
   else {
      str.clear();
   }

   return stream;
}



inline std::ostream& operator< (std::ostream& stream, bool val)               { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, char val)               { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, unsigned char val)      { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, signed char val)        { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, short val)              { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, unsigned short val)     { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, long val)               { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, unsigned long val)      { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, int val)                { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, unsigned int val)       { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, long long val)          { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, unsigned long long val) { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, float val)              { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, double val)             { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, long double val)        { return WriteToOstream(stream, &val, sizeof(val)); }
inline std::ostream& operator< (std::ostream& stream, const std::string& val) { return WriteStringToOStream(stream, val.c_str(), val.size()); }
inline std::ostream& operator< (std::ostream& stream, const char* val)        { return WriteStringToOStream(stream, val); }




inline std::istream& operator> (std::istream& stream, bool& val)               { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, char& val)               { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, unsigned char& val)      { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, signed char& val)        { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, short& val)              { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, unsigned short& val)     { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, long& val)               { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, unsigned long& val)      { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, int& val)                { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, unsigned int& val)       { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, long long& val)          { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, unsigned long long& val) { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, float& val)              { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, double& val)             { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, long double& val)        { return ReadFromIStream(stream, &val, sizeof(val)); }
inline std::istream& operator> (std::istream& stream, std::string& val)        { return ReadStringFromIStream(stream, val); }


template<typename T1, typename T2> std::ostream& operator< (std::ostream& stream, const std::pair<T1, T2>& val)
{
   stream < val.first < val.second;
   return stream;
}

template<typename T1, typename T2> std::istream& operator> (std::istream& stream, std::pair<T1, T2>& val)
{
   stream > val.first > val.second;
   return stream;
}

template<typename T> std::ostream& operator< (std::ostream& stream, const std::vector<T>& val)
{
   stream < val.size();
   for (auto it = val.begin(); it != val.end(); ++it) stream < *it;
   return stream;
}

template <typename T> std::istream& operator> (std::istream& stream, std::vector<T>& val)
{
   val.clear();

   size_t size;
   stream > size;
   if (!size) return stream;

   val.reserve(size);

   T t;
   for (size_t i = 0; i < size; ++i) {
      stream > t;
      val.push_back(t);
   }

   return stream;
}

inline std::ostream& operator< (std::ostream& stream, const std::vector<char>& v)
{
   stream < v.size();
   stream.write(&v[0], v.size());
   return stream;
}

inline std::istream& operator> (std::istream& stream, std::vector<char>& v)
{
   size_t size;
   stream > size;

   std::vector<char> tmp(size);
   if (size) stream.read(&tmp[0], size);
   v.swap(tmp);
   return stream;
}






