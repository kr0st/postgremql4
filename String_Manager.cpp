/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012-2019, Rostislav Kuratch                                      *
 * All rights reserved.                                                            */

#include "stdafx.h"

#include "String_Manager.h"


String_Manager::String_Manager(const size_t max_mem_usage):
max_mem_usage_(max_mem_usage),
current_mem_usage_(0)
{
}

String_Manager::~String_Manager()
{
    Mutex::Scoped_Lock lock(mutex_);
}

const wchar_t* String_Manager::managed_str(const std::string& str)
{
    Mutex::Scoped_Lock lock(mutex_);

    if (current_mem_usage_ > max_mem_usage_)
    {
        std::vector <std::wstring> empty;
        strings_.swap(empty);
        current_mem_usage_ = 0;
    }

    strings_.push_back(to_unicode(str));
    current_mem_usage_ += (str.length() * sizeof(wchar_t) + 1);

    return strings_.back().c_str();
}

void String_Manager::set_max_mem_usage(const size_t max_mem_usage)
{
    Mutex::Scoped_Lock lock(mutex_);
    max_mem_usage_ = max_mem_usage;
}

std::string String_Manager::to_ascii(const std::wstring& str)
{
    std::string temp;
    temp.resize(str.length());
    
    for (size_t i = 0; i < str.length(); ++i)
    {
        temp[i] = str[i];
    }

    return temp;
}

std::wstring String_Manager::to_unicode(const std::string& str)
{
    std::wstring temp;
    temp.resize(str.length());
    
    for (size_t i = 0; i < str.length(); ++i)
    {
        temp[i] = str[i];
    }

    return temp;
}
