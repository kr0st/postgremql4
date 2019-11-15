/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012-2019, Rostislav Kuratch                                      *
 * All rights reserved.                                                            */

#include "stdafx.h"

#include "postgremql4.h"
#include "Postgres_DB_Wrapper.h"
#include "String_Manager.h"

static DB_Wrapper_Interface* g_db_interface = 0;
static String_Manager g_str_mgr;
static Mutex g_dll_mutex;

POSTGREMQL4_API const wchar_t* pmql_connect(const wchar_t* whost, const wchar_t* wport, const wchar_t* wuser, const wchar_t* wpass, const wchar_t* wdb)
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (whost && wport && wuser && wpass && wdb)
    {
        std::string str_user = String_Manager::to_ascii(wuser), str_host = String_Manager::to_ascii(whost), str_pass = String_Manager::to_ascii(wpass), str_db = String_Manager::to_ascii(wdb);
        const char *user = str_user.c_str(), *host = str_host.c_str(), *pass = str_pass.c_str(), *db = str_db.c_str();

        char str[1024];
        sprintf_s(str, "user=%s host=%s password=%s dbname=%s", user, host, pass, db);
        
        if (g_db_interface)
            return g_str_mgr.managed_str("already connected");

        g_db_interface = new (std::nothrow) Postgres_DB_Wrapper;
        if (!g_db_interface)
            return g_str_mgr.managed_str("");

        return g_str_mgr.managed_str(g_db_interface->connect(str));
    }
    else
        return g_str_mgr.managed_str("");
}

POSTGREMQL4_API const wchar_t* pmql_disconnect()
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (!g_db_interface)
        return g_str_mgr.managed_str("ok");
    
    const wchar_t* res = g_str_mgr.managed_str(g_db_interface->disconnect());
    
    delete g_db_interface;
    g_db_interface = 0;
    
    return res;
}

POSTGREMQL4_API int pmql_begin_transaction()
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (!g_db_interface)
        return 0;

    return g_db_interface->begin_transaction();
}

POSTGREMQL4_API const wchar_t* pmql_commit(int trans_ptr)
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (!g_db_interface || (trans_ptr == 0))
        return g_str_mgr.managed_str("");

    return g_str_mgr.managed_str(g_db_interface->commit(trans_ptr));
}

POSTGREMQL4_API const wchar_t* pmql_rollback(int trans_ptr)
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (!g_db_interface || (trans_ptr == 0))
        return g_str_mgr.managed_str("");

    return g_str_mgr.managed_str(g_db_interface->rollback(trans_ptr));
}

POSTGREMQL4_API const wchar_t* pmql_exec_in_transaction(int trans_ptr, const wchar_t* query)
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (!g_db_interface || (trans_ptr == 0) || (query == 0))
        return g_str_mgr.managed_str("");

    return g_str_mgr.managed_str(g_db_interface->exec(trans_ptr, String_Manager::to_ascii(query)));
}

POSTGREMQL4_API const wchar_t* pmql_exec(const wchar_t* query)
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (!g_db_interface || (query == 0))
        return g_str_mgr.managed_str("");

    return g_str_mgr.managed_str(g_db_interface->exec(String_Manager::to_ascii(query)));
}

POSTGREMQL4_API void pmql_delimiters(int val_delim, int col_delim, int row_delim)
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (!g_db_interface)
        return;

    g_db_interface->set_delims((wchar_t)val_delim, (wchar_t)col_delim, (wchar_t)row_delim);
}

POSTGREMQL4_API void pmql_set_cache_size(int max_cache_size)
{
    Mutex::Scoped_Lock lock(g_dll_mutex);

    if (!g_db_interface)
        return;

    g_str_mgr.set_max_mem_usage((size_t)max_cache_size);
}

/*int main()
{
    std::wstring status = pmql_connect(L"www.bioreactor.me", L"5432", L"breakout", L"breakout", L"sentiment");

    int trans = pmql_begin_transaction();

    status = pmql_exec_in_transaction(trans, L"select * from \"EURUSD\" where (timestamp > '2014-12-29 12:55:00+02')");
    status = pmql_commit(trans);

    status = pmql_disconnect();

    return 0;
}*/

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
	switch (ul_reason_for_call)
	{
	    case DLL_PROCESS_ATTACH:
	    case DLL_THREAD_ATTACH:
	    case DLL_THREAD_DETACH:
	    case DLL_PROCESS_DETACH:
		    break;
	}

	return TRUE;
}
