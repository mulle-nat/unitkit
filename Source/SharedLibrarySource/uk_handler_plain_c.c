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
#include "uk_handler.h"
#include <stdio.h>
#include <stdlib.h>


int  __uk_run_test_in_pool( void (*f)( void), char *file, int line)
{
   // C has no pools
   __uk_complain_no_pool_available_once();
   return( __uk_run_test( f, file, line));
}


void   __uk_handler_objects_equal( void *a, void *b, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, a, b);
   abort();
}


void   __uk_handler_objects_not_equal( void *a, void *b, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, a, b);
   abort();
}


void   __uk_handler_objects_same( void *a, void *b, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, a, b);
   abort();
}


void   __uk_handler_objects_not_same( void *a, void *b, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, a, b);
   abort();
}


void   __uk_handler_strings_equal( void *a, void *b, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, a, b);
   abort();
}


void   __uk_handler_strings_not_equal( void *a, void *b, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, a, b);
   abort();
}


void   __uk_handler_string_contains( void *a, void *b, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, a, b);
   abort();
}


void   __uk_handler_string_does_not_contain( void *a, void *b, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, a, b);
   abort();
}


void   __uk_handler_does_not_raise_exception( void *exception, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p\n", __PRETTY_FUNCTION__, file, line, exception);
   abort();
}


void   __uk_handler_raises_exception( void *exception, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p\n", __PRETTY_FUNCTION__, file, line, exception);
   abort();
}


void   __uk_handler_raises_exception_class( void *exception, void *cls, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, exception, cls);
   abort();
}


void   __uk_handler_raises_exception_named( void *exception, void *name, char *file, int line)
{
   // no support for that in C
   fprintf( stdout, "%s %s:%d: %p %p\n", __PRETTY_FUNCTION__, file, line, exception, name);
   abort();
}
