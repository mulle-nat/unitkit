/*
 This source is part of UnitKit, a unit test framework for Mac OS X 
 development. You can find more information about UnitKit at:
 
 https://bitbucket.org/mulle_nat/unitkit-fixes
 
 Copyright (c) 2012 Nat!
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 The use of the Apache License does not indicate that this project is
 affiliated with the Apache Software Foundation.
 */
#include <assert.h> 
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "uk_handler.h"


struct __uk_handler_context   default_context;
struct __uk_handler_context   *__uk_handler_context = &default_context;
struct __uk_handler_context   *__uk_handler_shared_context;


#ifndef MAP_ANON
# define MAP_ANON  MAP_ANONYMOUS
#endif

static char  *printable_filename( char *file)
{
   char   *s;
   char   *filename;
   
   s        = file;
   filename = strrchr( s, '/');
   if( filename && filename[ 1])
      s = filename + 1;
   filename = strrchr( s, '\\');
   if( filename && filename[ 1])
      s = filename + 1;
   return( s);
}


// XXX we need to test these report messages as best as possible. Especially
// with the delegate set or not and responding to selector

static void   default_reporter( int passed, char *file, int line, char *message)
{
   char   *filename;
   
   if( passed && ! __uk_handler_context->not_quiet)
      return;
   
   filename = printable_filename( file);
   printf( "%s:%d %s %s\n", filename, line, message, passed ? "passes" : "fails");
   fflush( stdout);
}


static void   abort_reporter( int signal, char *file, int line)
{
   char   *filename;
   char   *name;
   
   filename = printable_filename( file);
   name     = "";

   switch( signal)
   {
   case SIGTRAP : name = "SIGTRAP "; break;
   case SIGFPE  : name = "SIGFPE ";  break;
   case SIGBUS  : name = "SIGBUS ";  break;
   case SIGABRT : name = "SIGABRT "; break;
   case SIGSEGV : name = "SIGSEGV "; break;
   }

   printf( "%s:%d %saborts\n", filename, line, name);
   fflush( stdout);
}


int   __uk_set_quiet( int quiet)
{
   int    old;
   
   old = ! __uk_handler_context->not_quiet;
   __uk_handler_context->not_quiet = ! quiet;
   return( old);
}


int   __uk_set_exit_on_failure( int exit_on_failure)
{
   int    old;
   
   old = ! __uk_handler_context->exit_on_failure;
   __uk_handler_context->exit_on_failure = exit_on_failure;
   return( old);
}


void   __uk_reset_statistics( void)
{
   memset( __uk_handler_context, 0, sizeof( struct __uk_handler_context));
}


int   __uk_show_statistics( void)
{
   struct __uk_handler_context   *p;
   
   p = __uk_handler_context;
   printf( "Result: %ld functions, %ld aborted, %ld tests, %ld failed \n", p->functions, p->aborts, p->tests, p->failures);
   return( p->failures + p->aborts ? -1 : 0);
}


//
// this runs the test in a separate process. the idea is that a result of 0
// is considered fine, everything else is an exception 
// 
static void   setup_shared_context_if_possible()
{
   if( ! __uk_handler_shared_context)
   {
      __uk_handler_shared_context = mmap( NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, 0, 0);
      if( __uk_handler_shared_context == MAP_FAILED)
      {
         __uk_handler_shared_context = NULL;
         fprintf( stderr, "can't acquire shared memory. statistics will be off\n");
         return;     // not such a big problem me thinks
      }
   }

   // voodoo check ;)
   if( __uk_handler_context != __uk_handler_shared_context)
   {
      memcpy( __uk_handler_shared_context, __uk_handler_context, sizeof( struct __uk_handler_context));
      __uk_handler_context = __uk_handler_shared_context;
   }   
}


static void   _just_exit( int signal)
{
   _exit( signal);
}



static int   run_test( void (*f)( void * userinfo), void *userinfo, char *file, int line)
{
   __uk_handler_context->functions++;
   (*f)( userinfo);
   return( 0);
}


static int   run_test_fork( void (*f)( void *userinfo), void *userinfo, char *file, int line)
{
   pid_t           pid;
   int             rval;
   int             status;
   unsigned long   tests;
   
   setup_shared_context_if_possible();
   
   __uk_handler_context->functions++;
   
   pid = fork();
   if( ! pid)
   {
      // child, problem is if child aborts we get an ugly
      // panel on OS-X. Do not want, so instead patch in _exit(1)
      // also catch some other common "crashes"
      
      signal( SIGTRAP, _just_exit);
      signal( SIGABRT, _just_exit);
      signal( SIGEMT,  _just_exit);
      signal( SIGFPE,  _just_exit);
      signal( SIGBUS,  _just_exit);
      signal( SIGSEGV, _just_exit);
      
      (*f)( userinfo);
      _exit( 0);
   }
   
   // voodoo loop 
   do
   {
      status = 0;
      rval   = wait( &status);
   }
   while( rval == -1);
   
   rval = WIFEXITED( status) ? WEXITSTATUS( status) : -1;

   if( rval)
   {
      abort_reporter( rval, file, line);
      __uk_handler_context->aborts++;
   }
   return( rval);
}


int   ___uk_run_test( void (*f)( void * userinfo), void *userinfo, char *file, int line)
{
   if( getenv( "UK_NO_FORK"))
      return( run_test( f, userinfo, file, line));
   return( run_test_fork( f, userinfo, file, line));
}


// needed because __uk_run_test is shared between objc and c
static void   bouncy_bounce( void (*f)( void))
{
   (*f)();
}


int   __uk_run_test( void (*f)( void), char *file, int line)
{
   return( ___uk_run_test( (void *) bouncy_bounce, f, file, line));
}


void   __uk_complain_no_pool_available_once()
{
   static char   flag;
   
   if( ! flag)
   {
      flag = 1;
      fprintf( stderr, "no autoreleasepool available in this environment\n");
   }
}


void  (*__uk_handler_reporter)( int passed, char *file, int line, char *message) =
   default_reporter;


void  __uk_handler_report_status( int passed, char *file, int line, char *message)
{
   __uk_handler_context->tests++;
   __uk_handler_context->failures += ! passed;
   
   (*__uk_handler_reporter)( passed, file, line, message);
   
   if( ! passed && __uk_handler_context->exit_on_failure)
      exit( 1);
}


void  __uk_handler_pass( char *file, int line)
{
   __uk_handler_report_status( 1, file, line, "");
}


void  __uk_handler_fail( char *file, int line)
{
   __uk_handler_report_status( 0, file, line, "");
}


void  __uk_handler_true( int condition, char *file, int line)
{
   __uk_handler_report_status( ! ! condition, file, line, "is true");
}


void  __uk_handler_false( int condition, char *file, int line)
{
   __uk_handler_report_status( ! condition, file, line, "is false");
}


void  __uk_handler_nil( void *ref, char *file, int line)
{
   __uk_handler_report_status( ! ref, file, line, "is nil/NULL");
}


void  __uk_handler_not_nil( void *ref, char *file, int line)
{
   __uk_handler_report_status( ! ! ref, file, line, "is not nil/NULL");
}


void  __uk_handler_ints_equal( long long a, long long b, char *file, int line)
{
   char   buf[ 256];
   char   *message;
   int    passed;
   
   passed = a == b;
   
   message = "integers equal";
   if( ! passed)
   {
      sprintf( buf, "%s (%lld == %lld)", message, a, b);
      message = buf;
   }
      
   __uk_handler_report_status( passed, file, line, message);
}


void  __uk_handler_ints_not_equal( long long a, long long b, char *file, int line)
{
   char   buf[ 256];
   char   *message;
   int    passed;
   
   passed = a != b;
   
   message = "integers not equal";
   if( ! passed)
   {
      sprintf( buf, "%s (%lld != %lld)", message, a, b);
      message = buf;
   }
   
   __uk_handler_report_status( passed, file, line, message);
}


void   __uk_handler_floats_equal( double a, double b, double d, char *file, int line)
{
   char     *message;
   char     buf[ 256];
   double   c;
   int      passed;
    
   c       = fabs( a - b);
   passed = (c <= d);
   
   message = "floats equal";
   if( ! passed)
   {
      sprintf( buf, "%s (%.18g == %.18g)", message, a, b);
      message = buf;
   }
   
   __uk_handler_report_status( passed, file, line, message);
}


void   __uk_handler_floats_not_equal( double a, double b, double d, char *file, int line)
{
   char     *message;
   char     buf[ 256];
   double   c;
   int      passed;
   
   c       = fabs( a - b);
   passed = (c > d);
   
   message = "floats not equal";
   if( ! passed)
   {
      sprintf( buf, "%s (%f != %f)", message, a, b);
      message = buf;
   }
   
   __uk_handler_report_status( passed, file, line, message);
   
}


void   __uk_handler_pointers_equal( void *a, void *b, char *file, int line)
{
   char   buf[ 256];
   char   *message;
   int    passed;
   
   passed = a == b;
   
   message = "pointers equal";
   if( ! passed)
   {
      sprintf( buf, "%s (%p == %p)", message, a, b);
      message = buf;
   }
   
   __uk_handler_report_status( passed, file, line, message);
}


void   __uk_handler_pointers_not_equal( void *a, void *b, char *file, int line)
{
   char   buf[ 256];
   char   *message;
   int    passed;
   
   passed = a != b;
   
   message = "pointers not equal";
   if( ! passed)
   {
      sprintf( buf, "%s (%p != %p)", message, a, b);
      message = buf;
   }
   
   __uk_handler_report_status( passed, file, line, message);
}


