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


#ifdef __MULLE_OBJC__
# include <objc-compat/objc-compat.h>
#else
# include <objc/objc.h>
# include <objc/runtime.h>
#endif


extern void  __uk_handler_report_status( int passed, char *file, int line, char *message);


typedef struct
{
   size_t   location;
   size_t   length;
} range;


//
// why not just .m and let the compiler do it ?
// I'd like to avoid touching the runtime system at all (esp. during link
// loading) until a test is actually run.
//
static Class  get_cls( id a)
{
   if( ! a)
      return( NULL);

   return( object_getClass( a));
}


static SEL   get_sel( char *sel_name)
{
   if( ! sel_name)
      return( 0);
   return( sel_getUid( sel_name));
}


static void  *get_imp( id a, SEL sel)
{
   Class   cls;
   Method  m;
   BOOL    (*imp)( id, SEL, id);

   cls = get_cls( a);
   if( ! cls || ! sel)
      return( NULL);

   m   = class_getInstanceMethod( cls, sel);
   if( ! m)
      return( NULL);

   return( method_getImplementation( m));
}


static void  *get_class_imp( Class cls, SEL sel)
{
   Method  m;
   BOOL    (*imp)( id, SEL, id);

   if( ! cls || ! sel)
      return( NULL);

   m   = class_getClassMethod( cls, sel);
   if( ! m)
      return( NULL);

   return( method_getImplementation( m));
}


static int   test_objects_bool( id a, char *sel_name, id b)
{
   BOOL   (*imp)( id, SEL, id);
   SEL    sel;

   sel = get_sel( sel_name);
   imp = get_imp( a, sel);
   if( ! imp)
      return( 0);

   return( (*imp)( a, sel, b));
}


static range   test_objects_range( id a, char *sel_name, id b)
{
   range   (*imp)( id, SEL, id);
   SEL     sel;

   sel = get_sel( sel_name);
   imp = get_imp( a, sel);
   if( ! imp)
      return( (range) { 0, 0 });
   return( (*imp)( a, sel, b));
}


static id   test_object_id( id a, char *sel_name)
{
   id    (*imp)( id, SEL);
   SEL   sel;

   sel = get_sel( sel_name);
   imp = get_imp( a, sel);
   if( ! imp)
      return( 0);

   return( (*imp)( a, sel));
}


static void   bouncy_bounce( void (*f)( void))
{
   Class  cls;
   SEL    sel;
   IMP    newImp;
   id     pool;
   int    rval;
   IMP    releaseImp;

   cls    = (Class) objc_getClass( "NSAutoreleasePool");
   sel    = get_sel( "new");
   newImp = get_class_imp( cls, sel);

   if( newImp)
      pool = objc_callIMP0( newImp, (id) cls, sel);

   (*f)();

   if( newImp)
   {
      sel        = get_sel( "release");
      releaseImp = get_imp( pool, sel);

      if( releaseImp)
         objc_callIMP0( releaseImp, pool, sel);
   }
}


int   __uk_run_test_in_pool( void (*f)( void), char *file, int line)
{
   extern void   __uk_complain_no_pool_available_once( void);
   extern int    ___uk_run_test( void (*)( void), void (*)( void), char *, int);

   if( ! (Class) objc_getClass( "NSAutoreleasePool"))
      __uk_complain_no_pool_available_once();

   return( ___uk_run_test( (void *) bouncy_bounce, f, file, line));
}


void   __uk_handler_objects_equal( void *a, void *b, char *file, int line)
{
   __uk_handler_report_status( test_objects_bool( a, "isEqual:", b), file, line, "objects equal");
}


void   __uk_handler_objects_not_equal( void *a, void *b, char *file, int line)
{
   __uk_handler_report_status( ! test_objects_bool( a, "isEqual:", b), file, line, "objects equal");
}


void   __uk_handler_objects_same( void *a, void *b, char *file, int line)
{
   __uk_handler_report_status( a == b, file, line, "objects same");
}


void   __uk_handler_objects_not_same( void *a, void *b, char *file, int line)
{
   __uk_handler_report_status( a != b, file, line, "objects same");
}


void   __uk_handler_strings_equal( void *a, void *b, char *file, int line)
{
   __uk_handler_report_status( test_objects_bool( a, "isEqualToString:", b), file, line, "strings equal");
}


void   __uk_handler_strings_not_equal( void *a, void *b, char *file, int line)
{
   __uk_handler_report_status( ! test_objects_bool( a, "isEqualToString:", b), file, line, "strings equal");
}


void   __uk_handler_string_contains( void *a, void *b, char *file, int line)
{
   __uk_handler_report_status( test_objects_range( a, "rangeOfString:", b).length, file, line, "string contains");
}


void   __uk_handler_string_does_not_contain( void *a, void *b, char *file, int line)
{
   __uk_handler_report_status( ! test_objects_range( a, "rangeOfString:", b).length, file, line, "string does not contain");
}


void   __uk_handler_does_not_raise_exception( void *exception, char *file, int line)
{
   __uk_handler_report_status( ! exception, file, line, "does not raise exception");
}


void   __uk_handler_raises_exception( void *exception, char *file, int line)
{
   __uk_handler_report_status( exception != NULL, file, line, "raises exception");
}


void   __uk_handler_raises_exception_class( void *exception, void *cls, char *file, int line)
{
   __uk_handler_report_status( test_objects_bool( exception, "isKindOfClass:", cls), file, line, "raises exception class");
}


void   __uk_handler_raises_exception_named( void *exception, void *name, char *file, int line)
{
   id   exception_name;

   exception_name = test_object_id( exception, "name");
   __uk_handler_report_status( test_objects_bool( exception_name, "isEqualToString:", name), file, line, "raises exception named");
}
