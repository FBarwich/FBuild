/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/*
 *
 * Author: Frank Barwich
 */
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <fstream>
#include <unordered_map>
#include <iterator>

#include <cstdlib>
#include <ctime>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>


#pragma warning(disable:4355)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#include <v8.h>
#pragma warning(default:4355)
#pragma warning(default:4100)
#pragma warning(default:4127)
