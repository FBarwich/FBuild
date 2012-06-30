/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "FileOutOfDate.h"

#include <boost/filesystem.hpp>


bool FileOutOfDate::Go ()
{
   if (!boost::filesystem::exists(parent)) return true;

   std::time_t parentTime = boost::filesystem::last_write_time(parent);

   for (size_t i = 0; i < files.size(); ++i) {
      if (boost::filesystem::last_write_time(files[i]) > parentTime) return true;
   }

   return false;
}
