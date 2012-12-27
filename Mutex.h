/* Distributed under the BSD license (http://opensource.org/licenses/BSD-2-Clause) *
 * Copyright (c) 2012, Rostislav Kuratch                                           *
 * All rights reserved.                                                            */

#pragma once

/*! \brief
 * A synchronization primitive for multithreaded applications.
 * Multiple Get() method calls from one process will not lead to deadlock,
 * in other words the mutex is reentrant.
 */
class Mutex
{
	public:

        //! Mutex is acquired on Scoped_Lock construction and released on destruction.
        class Scoped_Lock
        {
            public:

                //! Constructor, takes in reference to Mutex object that will be controlled.
                Scoped_Lock( Mutex& mutex ):
                mutex_( mutex )
                {
                    mutex_.Get();
                }

                //! Destructor, releases Mutex object.
                virtual ~Scoped_Lock() { mutex_.Release(); }

            protected:

                Mutex& mutex_;
        };

        //! Constructor, creates unnamed Mutex object.
        Mutex() { InitializeCriticalSection( &cs_ ); }

        ~Mutex() { DeleteCriticalSection( &cs_ ); }

        /*! \brief
         * Gets the mutex. If it is not free then waits (blocking) for specified timeout
         * and either gets the mutex or returns an error. If no error was encountered returns 0.
         * Please note that timeout value has no effect now because it is not implemented.
         */
        unsigned int Get( unsigned int timeout = INFINITE )
        {
            timeout = timeout;
            EnterCriticalSection( &cs_ );
            return 0;
        }

        //! Releases the mutex allowing other threads to access the mutex-protected resource.
        unsigned int Release()
        {
            LeaveCriticalSection( &cs_ );
            return 0;
        }


	protected:

		CRITICAL_SECTION cs_;
        Mutex( Mutex& );
};
