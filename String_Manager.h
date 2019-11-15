/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012-2019, Rostislav Kuratch                                      *
 * All rights reserved.                                                            */

#pragma once

#include "Mutex.h"

class String_Manager
{
    public:

        String_Manager(const size_t max_mem_usage = 1024 * 1024 * 3); //3 MB mem use limit, after reaching it cache is cleared
        virtual ~String_Manager();
        const wchar_t* managed_str(const std::string& str);
        void set_max_mem_usage(const size_t max_mem_usage);

        static std::string to_ascii(const std::wstring& str);
        static std::wstring to_unicode(const std::string& str);


    private:

        Mutex mutex_;
        size_t max_mem_usage_;
        size_t current_mem_usage_;
        std::vector <std::wstring> strings_;
};
