//+------------------------------------------------------------------+
//|                                                                  Ticks Collector.mq4 |
//|                                                                     Rostislav Kuratch |
//|                                                      rostislav.kuratch@gmail.com |
//+------------------------------------------------------------------+
#property copyright "Rostislav Kuratch"
#property link "rostislav.kuratch@gmail.com"

#include <postgremql4.mqh>


string g_db_ip_setting = "x.x.x.x";
string g_db_port_setting = "5432";
string g_db_user_setting = "your_db_user";
string g_db_password_setting = "your_db_password";
string g_db_name_setting = "your_db_name";

//Current time zone of your broker (you should clarify this with your broker, +01 is just an example),
//please remember to change it when daylight savings are in effect, of course *only* if your broker observes DST
string g_timezone_setting = "+01";


bool is_error(string str)
{
    return (StringFind(str, "error") != -1);
}

bool prepare_db()
{
    string test_query = "SELECT * FROM \"" + Symbol() + "\" LIMIT 1";
    string res = pmql_exec(test_query);
    
    if (!is_error(res))
    {
        Print("Table " + Symbol() + " already exists.");
        return (true);
    }
    
    if (StringFind(res, "does not exist") > 0)
    {
        Print("Table " + Symbol() + " does not exist, will create.");
        
        string create_query = "CREATE TABLE \"" + Symbol() +"\" (bid numeric(10,5) NOT NULL, ask numeric(10,5) NOT NULL, \"timestamp\" timestamp with time zone NOT NULL, CONSTRAINT \"" + Symbol() +"_pkey\" PRIMARY KEY (\"timestamp\", bid, ask)) WITH (OIDS=FALSE);";
        res = pmql_exec(create_query);
        
        if (is_error(res))
        {
            Print(res);
            return (false);
        }

        return (true);
    }
    else
    {
        Print(res);
        return (false);
    }
    
    return (false);
}

int init()
{
    string res = pmql_connect(g_db_ip_setting, g_db_port_setting, g_db_user_setting, g_db_password_setting, g_db_name_setting);
    if ((res != "ok") && (res != "already connected"))
    {
        Print("DB not connected!");
        return (-1);
    }

    if (!prepare_db())
        return (-1);
    
    return(0);
}

int deinit()
{
    pmql_disconnect();
    return(0);
}

int start()
{
    string time = TimeToStr(TimeCurrent(), TIME_DATE | TIME_SECONDS);
    time = StringSetChar(time, 4, '-');
    time = StringSetChar(time, 7, '-');
    
    int ms = GetTickCount();
    if (ms < 0) ms = (-1 * ms);
    ms = ms % 1000;
    string ms_str = ms;
    
    while (StringLen(ms_str) < 3)
        ms_str = ("0" + ms_str);

    time = time + "." + ms_str;

    string query = "INSERT INTO \"" + Symbol() + "\" (timestamp, bid, ask) VALUES ('" + time + g_timezone_setting + "', " + DoubleToStr(NormalizeDouble(Bid, 5), 5) + ", " + DoubleToStr(NormalizeDouble(Ask, 5), 5) + ")";
    string res = pmql_exec(query);

    if (is_error(res))
        Print(res);

    return(0);
}
