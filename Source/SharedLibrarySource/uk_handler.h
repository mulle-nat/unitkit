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

struct __uk_handler_context
{
   unsigned long   functions;
   unsigned long   tests;
   unsigned long   failures;
   unsigned long   aborts;
   int             exit_on_failure;
   int             not_quiet;
};


extern struct __uk_handler_context     *__uk_handler_context;

extern void  (*__uk_handler_reporter)( int passed, char *file, int line, char *message);

extern void  __uk_handler_report_status( int passed, char *file, int line, char *message);

extern int   __uk_run_test( void (*f)( void), char *file, int line);
extern int   __uk_run_test_in_pool( void (*f)( void), char *file, int line);
extern void  __uk_reset_statistics( void);
extern int   __uk_show_statistics( void);
extern int   __uk_set_quiet( int quiet);
extern int   __uk_set_exit_on_failure( int exit_on_failure);
extern void  __uk_complain_no_pool_available_once( void);

// private
extern int   ___uk_run_test( void (*f)( void * userinfo), void *userinfo, char *file, int line);
