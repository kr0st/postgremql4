/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012, Rostislav Kuratch                                           *
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

const char* String_Manager::managed_str(const std::string& str)
{
    Mutex::Scoped_Lock lock(mutex_);

    if (current_mem_usage_ > max_mem_usage_)
    {
        std::vector <std::string> empty;
        strings_.swap(empty);
        current_mem_usage_ = 0;
    }

    strings_.push_back(str);
    current_mem_usage_ += (str.length() + 1);

    return strings_.back().c_str();
}

void String_Manager::set_max_mem_usage(const size_t max_mem_usage)
{
    Mutex::Scoped_Lock lock(mutex_);
    max_mem_usage_ = max_mem_usage;
}
