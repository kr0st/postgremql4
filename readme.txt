PostgreMQL4 DLL enables MQL4 script running inside MetaTrader 4 trading terminal
to communicate with PostgreSQL DB.

Why you might want to try this?
There are at least 2 things you could achieve:

    * store all quotes coming from your broker to DB in order to have some proof in case
      of a dispute or to invent/test your own trading system that needs
      high frequency data;

    * let�s say you already have a good trading system and you need an interface between
      your trading system and MT4, now you have it � PostgreSQL database,
      from one side trading system will write signals to DB and from the other side
      an expert will be running in MT4 using postgremql4.dll for DB access.


Supported Platforms

Windows XP+ and any version of MT4 terminal running on it.


License

BSD 2-Clause (http://opensource.org/licenses/BSD-2-Clause).


Known issues

Currently only 1 DB connection per MT4 terminal is supported, meaning all EAs running in
a single terminal instance will share this connection. This might produce unexpected side-
effects:
imagine EA#1 connects to DB �foo� and EA#2 connects to DB �bar�, EA#1 was quicker
so the connection happens with DB �foo� and EA#2 just gets response �already connected�.

However in case of EA#2 �already connected� does not mean that connection really happened
with DB �bar�, it just means that something already made a DB connection.
Also if EA#1 and EA#2 used different credentials, in our example connection was established
using credentials of EA#1 only, so EA#2 has the same access level to DB as EA#1.

Finally if both �foo� and �bar� DBs have table named �foobar� then both EAs will be
writing to �foo.foobar� table and of course that is unexpected because
the expectation was that EA#1 writes to �foo.foobar� and EA#2 writes to �bar.foobar�.


Compile

After you downloaded sources search for archive third_party.7z and extract its content at
the source root folder.
After extracting files you should have directories [source root]/third_party/libpqxx and
[source root]/third_party/licenses. First folder contains libpqxx headers, binaries and
dependencies (some PostgreSQL client libs), second folder has license information on
libpqxx and dependencies taken from PostgreSQL project.
Now you are ready to build PostgreMQL4, to do this simply open solution file with
MS Visual Studio 2010 (Express edition will do) and build the solution.


Install

Please install Microsoft Visual C++ 2010 Redistributable Package (x86) from here:
http://www.microsoft.com/en-us/download/details.aspx?id=5555
Scroll down to Version History and download the latest binary release using the link
provided there.

For v1.0.0.1:

Extract release archive directly into MT4 client directory, you will need administrator
permissions and you will be asked if you would like to merge folders,
answer �yes�.

For v1.1.0.0:

Folder structure has changed significantly with build 600 and now the archive
contains 2 folders, each should be copied on top of the existing folder.

Content of the folder called "to mt4 install folder" should be copied directly into
MT4 install dir.

Content of "to appdata roaming" should be copied to a special temp folder deep inside
the Windows user home directory. This is the directory MT4 now uses for storing all
user files including EAs. Here is example of the location of this folder on my PC:
"C:\Users\kr0st\AppData\Roaming\MetaQuotes\Terminal\172BCC25D314F0E906794B8ECF7BFC66".

As with the previous version just overwrite the existing folder with the one from the
archive, this way you will merge old content of your MQL4 folder with postgremql4 library.

Example Expert Advisor

To illustrate PostgreMQL4 functionality there is �Ticks Collector� expert advisor inside
the samples folder.
Easiest way to try it is to download the binary release and install it as described above.
If you would like to store ticks from your broker you will need to setup R/W access to
some DB for the EA then configure parameters in the expert�s source code and recompile
the expert, however there is no need to create tables - EA will create table using
the name of the instrument it is attached to.
You should open charts for all instruments you would like to store and drop this EA to
each of these charts also making sure DLL usage is permitted from MQL4 scripts.


Library Interface (API)

First thing to mention is that this project is a wrapper around libpqxx usable from MQL4,
so you could benefit from looking at libpqxx API here:
http://pqxx.org/devprojects/libpqxx/doc/4.0/html/Reference/

Function return types and error handling.

Only strings and integers could be returned by this library. This is the limitation of
MQL4 that cannot operate any data structures that are more complex than a number,
a string or an array.
That is why strings are used for returning query results in a way very similar to CSV
files but here we need to be able to distinct rows, columns inside those rows and values
assigned to these columns inside rows, therefore 3 types of separators are used:
row separator (character �*� by default), column separator (character �|� by default)
and value separator (character �=� by default).

For example this would be a correct query result (3 rows, 2 columns each):
name=Janelle|surname=Voight*name=Todd|surname=Voight*name=John|surname=Connor

There is a general rule how functions report errors, in case some exception
happened inside PostgreMQL4 DLL, you will get a string that looks like this one:
error*Cannot connect to DB

if you did not change row delimiter using API �*� character serves as the row delimiter,
so in case of an error you will always get 2 rows where the first row is always �error�
and the second row contains human-readable exception description like
�Cannot connect to DB�.

Below is the list of functions exported from PostgreMQL4 DLL with descriptions.

const char* pmql_connect(const wchar_t* host, const wchar_t* port,
                         const wchar_t* user, const wchar_t* pass, const wchar_t* db)
    
    Connects to the database.
    Arguments:
        host - hostname or IP
        port - port which PostgreSQL listens
        user - DB user name
        pass - password for a given DB user
        db - database name to connect to
    Returns: �ok� or �already connected� on success, anything else means error.

const char* pmql_disconnect()

    Disconnects from database.
    Returns: �ok� on success, anything else means error.

int pmql_begin_transaction()

    Begins DB transaction.
    Returns:
        Transaction token > 0 on success or 0 on error. This number is actually
        a pointer to a transaction object converted to a number.
        MQL4 has no concept of pointers thus transaction object is created inside DLL
        and tied with queries via integer transaction token that should be passed to
        other API functions and inside those functions converted back to the pointer
        to the transaction object.
        Only 1 transaction is allowed at a time, all calls to this function after
        the first successful one will return 0 until commit or rollback.

const char* pmql_commit(int trans_ptr)

    Commits the transaction identified by trans_ptr, which is a transaction token
    acquired from pmql_begin_transaction().
    Returns: �ok� on success, anything else means error.

const char* pmql_rollback(int trans_ptr)

    Rollbacks the transaction identified by trans_ptr, which is a transaction token
    acquired from pmql_begin_transaction().
    Returns: �ok� on success, anything else means error.

const char* pmql_exec_in_transaction(int trans_ptr, const wchar_t* query)

    Executes query inside the transaction identified by trans_ptr, which is a
    transaction token acquired from pmql_begin_transaction(). 
    Returns: delimitered query result containing 0 or more rows on success
             or error description on failure.
             Empty string is also considered a success meaning that query did not
             return anything.

const char* pmql_exec(const wchar_t* query)

    Executes query in auto-commit mode.
    Returns: delimitered query result containing 0 or more rows on success or
             error description on failure.
	         Empty string is also considered a success meaning that query did not
             return anything.

pmql_delimiters(int val_delim, int col_delim, int row_delim);
    
    Changes default delimiters.
    Arguments:
        val_delim - character that separates column name from column value (default �=�)
        col_delim - character that separates one colname=value pair from another
                    colname=value pair, in other words it tells where value of one column
                    ends and a name of another column begins (default �|�)
        row_delim - character that separates list of colname=value pairs from another
                    list of colname=value pairs, in other words it divides lists of
                    columns with values into rows (default �*�)

void pmql_set_cache_size(int max_cache_size)

    Changes the default 3MB memory chunk used to store query result
    (supplied argument value should be in bytes).
    
    Every time a string is returned from DLL to MQL4 script or expert advisor we have
    a memory leak because nobody releases memory used by the returned string.
    There is no automatic memory management in C/C++ and MQL4 does not support complex
    return types (like std::string) that do not need a special care.
    One way to overcome this is to always return a string together with a special number
    which is used after string is no longer needed in some function like
    free_mem(int token).
    
    Another variant is to remember all allocated string pointers and free memory used by
    the strings when memory usage grows beyond a given threshold, some simple kind of
    garbage collection.
    This library uses the second variant because it is more convenient for the library
    user and is less error-prone.
    Please note that it is absolutely ok to get a query result which takes more space
    than max_cache_size, it means only that when the next query is executed
    garbage collector will first free previously allocated memory and only then supply
    new query result.


Version History

v1.1.1.0 (http://kr0st.users.sourceforge.net/files/v1.1.1.0-postgremql4.7z)
Switched to the latest stable libpqxx (6.4) and the latest possible 32bit client library of PostgreSQL (10.9).
Code building switched to Visual Studio 2019.

v1.1.0.0 (http://kr0st.users.sourceforge.net/files/v1.1.0.0-postgremql4.7z)
Compatibility with MT4 Build 600+ added.

Strings are in "Unicode" now, however this is just a compatibility fix
meaning no new features were added. Therefore the library still supports only ASCII
but strings are passed between MT4 and library in 2 bytes per character format
in order to prevent crashes related to memory issues.

v1.0.0.1 (http://kr0st.users.sourceforge.net/files/v1.0.0.1-postgremql4.7z)
Initial release.
