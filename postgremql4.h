/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012, Rostislav Kuratch                                           *
 * All rights reserved.                                                            */

#pragma once

#ifdef POSTGREMQL4_EXPORTS
#define POSTGREMQL4_API extern "C" __declspec(dllexport)
#else
#define POSTGREMQL4_API extern "C" __declspec(dllimport)
//#define POSTGREMQL4_API
#endif

POSTGREMQL4_API const char* pmql_connect(const char* host, const char* port, const char* user, const char* pass, const char* db);
POSTGREMQL4_API const char* pmql_disconnect();

POSTGREMQL4_API int pmql_begin_transaction();
POSTGREMQL4_API const char* pmql_commit(int trans_ptr);
POSTGREMQL4_API const char* pmql_rollback(int trans_ptr);

POSTGREMQL4_API const char* pmql_exec_in_transaction(int trans_ptr, const char* query);
POSTGREMQL4_API const char* pmql_exec(const char* query);

POSTGREMQL4_API void pmql_delimiters(int val_delim, int col_delim, int row_delim);
POSTGREMQL4_API void pmql_set_cache_size(int max_cache_size);