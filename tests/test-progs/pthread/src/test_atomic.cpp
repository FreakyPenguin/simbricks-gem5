/*
  * Copyright (c) 2018, Cornell University
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or
  * without modification, are permitted provided that the following
  * conditions are met:
  *
  * Redistributions of source code must retain the above copyright
  * notice, this list of conditions and the following disclaimer.
  *
  * Redistributions in binary form must reproduce the above
  * copyright notice, this list of conditions and the following
  * disclaimer in the documentation and/or other materials provided
  * with the distribution.
  *
  * Neither the name of Cornell University nor the names of its
  * contributors may be used to endorse or promote products derived
  * from this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
  * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  * POSSIBILITY OF SUCH DAMAGE.
  */

#include <pthread.h>

#include <atomic>
#include <cstdlib>
#include <iostream>

//------------------------------------------------------------------------
// Create n threads, run them in parallel and wait for them in the master
// thread.
// Each child thread increments a shared variable m times atomically
//------------------------------------------------------------------------

#define MAX_N_WORKER_THREADS 10

typedef struct
{
    int nsteps;
    std::atomic<int>* shared_var;
} ThreadArg;

void* func( void* args )
{
    ThreadArg* my_args = ( ThreadArg* ) args;

    int nsteps = my_args->nsteps;
    std::atomic<int>* shared_var = my_args->shared_var;

    for ( int i = 0; i < nsteps; ++i ) {
        std::atomic_fetch_add(shared_var, 1);
    }

    return nullptr;
}

int main( int argc, const char* argv[] )
{
    int n_worker_threads = 0;

    // allocate all threads
    pthread_t* threads = new pthread_t[MAX_N_WORKER_THREADS];

    // variable shared among all threads
    std::atomic<int> shared_var(0);

    // number of steps each thread increments the shared_var
    int nsteps = 1000;

    // set up threads' arguments
    ThreadArg* t_args = new ThreadArg[MAX_N_WORKER_THREADS];

    int ret = 0;
    for ( size_t tid = 0; tid < MAX_N_WORKER_THREADS; tid++ ){
        t_args[tid].nsteps = nsteps;
        t_args[tid].shared_var = &shared_var;

        // spawn thread
        ret = pthread_create( threads + tid, nullptr, func, &t_args[tid] );

        if (ret != 0) {
            break;
        }

        n_worker_threads++;
    }

    // sync up all threads
    for ( int tid = 0; tid < n_worker_threads; ++tid ) {
        pthread_join( threads[tid], nullptr );
    }

    // clean up
    delete[] threads;
    delete[] t_args;

    // verify
    if ( shared_var != n_worker_threads * nsteps || n_worker_threads < 1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}