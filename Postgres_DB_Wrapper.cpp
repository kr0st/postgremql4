/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012-2014, Rostislav Kuratch                                      *
 * All rights reserved.                                                            */

#include "stdafx.h"

#include "Postgres_DB_Wrapper.h"


Postgres_DB_Wrapper::Postgres_DB_Wrapper():
DB_Wrapper_Interface(),
db_connection_(0)
{
}

Postgres_DB_Wrapper::~Postgres_DB_Wrapper()
{
    Mutex::Scoped_Lock lock(mutex_);

    rollback_existing_transactions();

    delete db_connection_;
    db_connection_ = 0;
}

std::string Postgres_DB_Wrapper::connect(const std::string& connection_string)
{
    Mutex::Scoped_Lock lock(mutex_);

    if (db_connection_)
    {
        rollback_existing_transactions();
        delete db_connection_;
    }
    try
    {
        db_connection_ = new (std::nothrow) pqxx::connection(connection_string);
        if (!db_connection_)
            return make_err_str("Cannot create DB connection, out of memory?");
        return "ok";
    }
    catch (std::exception& e)
    {
        return make_err_str(e.what());
    }

    return make_err_str("Undefined error.");
}

int Postgres_DB_Wrapper::begin_transaction()
{
    Mutex::Scoped_Lock lock(mutex_);

    try
    {
        pqxx::work* trans = new (std::nothrow) pqxx::work(*db_connection_);
        transactions_.push_back(trans);
        return (int)trans;
    }
    catch (std::exception&)
    {
        return 0;
    }

    return 0;
}

std::string Postgres_DB_Wrapper::commit(const int trans_ptr)
{
    Mutex::Scoped_Lock lock(mutex_);

    try
    {
        pqxx::work* trans = dynamic_cast <pqxx::work*> ((pqxx::basic_transaction*)trans_ptr);
        if (!trans)
            return make_err_str("Provided token was not a transaction.");

        trans->commit();

        remove_transaction(trans);

        return "ok";
    }
    catch (std::exception& e)
    {
        return make_err_str(e.what());
    }

    return make_err_str("Undefined error.");
}

std::string Postgres_DB_Wrapper::rollback(const int trans_ptr)
{
    Mutex::Scoped_Lock lock(mutex_);

    try
    {
        pqxx::work* trans = dynamic_cast <pqxx::work*> ((pqxx::basic_transaction*)trans_ptr);
        if (!trans)
            return make_err_str("Provided token was not a transaction.");
        
        trans->abort();

        remove_transaction(trans);

        return "ok";
    }
    catch (std::exception& e)
    {
        return make_err_str(e.what());
    }

    return make_err_str("Undefined error.");
}

std::string Postgres_DB_Wrapper::exec(const int trans_ptr, const std::string& query)
{
    Mutex::Scoped_Lock lock(mutex_);

    if (!db_connection_)
        return make_err_str("DB connection not available.");

    try
    {
        pqxx::work* trans = dynamic_cast <pqxx::work*> ((pqxx::basic_transaction*)trans_ptr);
        if (!trans)
            return make_err_str("Provided token was not a transaction.");

        return exec(trans, query);
    }
    catch (std::exception& e)
    {
        return make_err_str(e.what());
    }

    return make_err_str("Undefined error.");
}

std::string Postgres_DB_Wrapper::exec(const std::string& query)
{
    Mutex::Scoped_Lock lock(mutex_);

    if (!db_connection_)
        return make_err_str("DB connection not available.");

    try
    {
        pqxx::work work(*db_connection_);
        std::string res(exec(&work, query));
        
        std::string err_str("error ");
        err_str[err_str.length() - 1] = row_delim_;
        
        if (res.find(err_str) == std::string::npos)
            work.commit();
        else
            work.abort();

        return res;
    }
    catch (std::exception& e)
    {
        return make_err_str(e.what());
    }

    return make_err_str("Undefined error.");
}

std::string Postgres_DB_Wrapper::disconnect()
{
    Mutex::Scoped_Lock lock(mutex_);
    
    if (!db_connection_)
        return make_err_str("DB connection not available.");

    try
    {
        rollback_existing_transactions();
        db_connection_->disconnect();

        delete db_connection_;
        db_connection_ = 0;

        return "ok";
    }
    catch (std::exception& e)
    {
        return make_err_str(e.what());
    }

    return make_err_str("Undefined error.");
}

void Postgres_DB_Wrapper::rollback_existing_transactions()
{
    std::vector <pqxx::work*>::const_iterator it(transactions_.begin()), end(transactions_.end());
    for (; it != end; ++it)
        delete *it;

    std::vector <pqxx::work*> empty;
    transactions_.swap(empty);
}

void Postgres_DB_Wrapper::remove_transaction(const pqxx::work* trans)
{
    std::vector <pqxx::work*> new_trans_vector;

    std::vector <pqxx::work*>::const_iterator it(transactions_.begin()), end(transactions_.end());
    for (; it != end; ++it)
        if (*it != trans)
            new_trans_vector.push_back(*it);

    delete trans;

    transactions_.swap(new_trans_vector);
}

std::string Postgres_DB_Wrapper::exec(pqxx::work* trans, const std::string& query)
{
    if (!trans)
        return make_err_str("DB transaction not available.");
    
    std::string val_delim(" "), col_delim(" "), row_delim(" ");
    val_delim[0] = val_delim_;
    col_delim[0] = col_delim_;
    row_delim[0] = row_delim_;

    std::string res_str = "";

    try
    {
        pqxx::work& work(*trans);

        pqxx::result result(work.exec(query));

        pqxx::result::const_iterator rows_it(result.begin());
        pqxx::result::const_iterator rows_end(result.end());

        int curr_row = 0;
        for (; rows_it != rows_end; ++rows_it)
        {
            pqxx::tuple::const_iterator cols_it(rows_it->begin());
            pqxx::tuple::const_iterator cols_end(rows_it->end());

            int curr_col = 0;
            for (; cols_it != cols_end; ++cols_it)
            {
                res_str += (cols_it->name() + val_delim + std::string(cols_it->c_str()));
                ++curr_col;
                if (curr_col < (int)rows_it->size())
                    res_str += col_delim;
            }

            ++curr_row;
            if (curr_row < (int)result.size())
                res_str += row_delim;
        }

        return res_str;
    }
    catch (std::exception& e)
    {
        return make_err_str(e.what());
    }

    return make_err_str("Undefined error.");
}
