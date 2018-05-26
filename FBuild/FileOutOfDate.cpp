/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "FileOutOfDate.h"

#include <filesystem>


bool FileOutOfDate::Go () const
{
   if (!std::filesystem::exists(parent)) return true;

   const auto parentTime = std::filesystem::last_write_time(parent);

   for (size_t i = 0; i < files.size(); ++i) {
      if (std::filesystem::last_write_time(files[i]) > parentTime) return true;
   }

   return false;
}
