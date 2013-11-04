/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include "JavaScript.h"

#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

int main (int argc, char** argv)
{
   try {
      std::vector<std::string> args;
      for (int i = 1; i < argc; ++i) args.push_back(argv[i]);

      ::SetPriorityClass(::GetCurrentProcess(), IDLE_PRIORITY_CLASS);

      JavaScript js(args);
      js.ExecuteFile("FBuild.js");
      return 0;
   }
   catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }
}