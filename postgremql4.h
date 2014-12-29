/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012-2014, Rostislav Kuratch                                      *
 * All rights reserved.                                                            */

#pragma once

#ifdef POSTGREMQL4_EXPORTS
#define POSTGREMQL4_API extern "C" __declspec(dllexport)
#else
#define POSTGREMQL4_API extern "C" __declspec(dllimport)
#endif

POSTGREMQL4_API const wchar_t* pmql_connect(const wchar_t* host, const wchar_t* port, const wchar_t* user, const wchar_t* pass, const wchar_t* db);
POSTGREMQL4_API const wchar_t* pmql_disconnect();

POSTGREMQL4_API int pmql_begin_transaction();
POSTGREMQL4_API const wchar_t* pmql_commit(int trans_ptr);
POSTGREMQL4_API const wchar_t* pmql_rollback(int trans_ptr);

POSTGREMQL4_API const wchar_t* pmql_exec_in_transaction(int trans_ptr, const wchar_t* query);
POSTGREMQL4_API const wchar_t* pmql_exec(const wchar_t* query);

POSTGREMQL4_API void pmql_delimiters(int val_delim, int col_delim, int row_delim);
POSTGREMQL4_API void pmql_set_cache_size(int max_cache_size);