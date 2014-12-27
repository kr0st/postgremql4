/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012-2014, Rostislav Kuratch                                      *
 * All rights reserved.                                                            */

#pragma once

#define NOMINMAX
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define WIN32_LEAN_AND_MEAN

#include <SDKDDKVer.h>

#include <stdio.h>
#include <pqxx/pqxx>
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>