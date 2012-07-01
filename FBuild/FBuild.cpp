/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */

#include <iostream>

#include "Script.h"

int main (int argc, char** argv)
{
   try {
      Script script(argc, argv);
      script.ExecuteFile("FBuild.lua");

      return 0;
   }
   catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      return 5;
   }
}