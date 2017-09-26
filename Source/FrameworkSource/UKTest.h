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


#ifdef __OBJC__
/*!
 @protocol UKTest
 @abstract The marker protocol that indicates that a class should be picked up by a system.
 @discussion (description)
*/
@protocol UKTest

@end
#endif


/* original macros rewritten to use C bouncers.
 * 2012 Nat!, Mulle kybernetiK 
 * Advantages are:
 * with a different library same code can be used in plain C code
 * somewhat smaller generated test binaries :)
 *
 * Stay Frosty!
 */
 
extern void  __uk_handler_pass( char *file, int line);
extern void  __uk_handler_fail( char *file, int line);

#define UKPass()   __uk_handler_pass( __FILE__, __LINE__)
#define UKFail()   __uk_handler_fail( __FILE__ , __LINE__)

extern void   __uk_handler_true( int condition, char *file, int line);
extern void   __uk_handler_false( int condition, char *file, int line);

#define UKTrue( condition)    __uk_handler_true( (condition), __FILE__, __LINE__);
#define UKFalse( condition)   __uk_handler_false( (condition), __FILE__, __LINE__);

extern void   __uk_handler_nil( void *condition, char *file, int line);
extern void   __uk_handler_not_nil( void *condition, char *file, int line);

#define UKNil( ref)      __uk_handler_nil( (ref), __FILE__, __LINE__) 
#define UKNotNil( ref)   __uk_handler_not_nil( (ref), __FILE__, __LINE__)

extern void  __uk_handler_ints_equal( long long a, long long b, char *file, int line);
extern void  __uk_handler_ints_not_equal( long long a, long long b, char *file, int line);

#define UKIntsEqual( a, b)      __uk_handler_ints_equal( (a), (b) , __FILE__, __LINE__)
#define UKIntsNotEqual( a, b)   __uk_handler_ints_not_equal( (a), (b), __FILE__, __LINE__)

extern void  __uk_handler_floats_equal( double a, double b, double d, char *file, int line);
extern void  __uk_handler_floats_not_equal( double a, double b, double d, char *file, int line);

#define UKFloatsEqual( a, b, d)      __uk_handler_floats_equal( (a), (b), (d), __FILE__, __LINE__)
#define UKFloatsNotEqual( a, b, d)   __uk_handler_floats_not_equal( (a), (b), (d), __FILE__, __LINE__)

extern void  __uk_handler_objects_equal( void *a, void *b, char *file, int line);
extern void  __uk_handler_objects_not_equal( void *a, void *b, char *file, int line);

#define UKObjectsEqual( a, b)      __uk_handler_objects_equal( (a), (b) , __FILE__, __LINE__)
#define UKObjectsNotEqual( a, b)   __uk_handler_objects_not_equal( (a), (b), __FILE__, __LINE__)

extern void  __uk_handler_objects_same( void *a, void *b, char *file, int line);
extern void  __uk_handler_objects_not_same( void *a, void *b, char *file, int line);

#define UKObjectsSame( a, b)      __uk_handler_objects_same( (a), (b), __FILE__, __LINE__)
#define UKObjectsNotSame( a, b)   __uk_handler_objects_not_same( (a), (b), __FILE__, __LINE__)

extern void  __uk_handler_pointers_equal( void *a, void *b, char *file, int line);
extern void  __uk_handler_pointers_not_equal( void *a, void *b, char *file, int line);

#define UKPointersEqual( a, b)      __uk_handler_pointers_equal( (a), (b), __FILE__, __LINE__)
#define UKPointersNotEqual( a, b)   __uk_handler_pointers_not_equal( (a), (b), __FILE__, __LINE__)

extern void  __uk_handler_strings_equal( void *a, void *b, char *file, int line);
extern void  __uk_handler_strings_not_equal( void *a, void *b, char *file, int line);

#define UKStringsEqual( a, b)      __uk_handler_strings_equal( (a), (b), __FILE__, __LINE__)
#define UKStringsNotEqual( a, b)   __uk_handler_strings_not_equal( (a), (b), __FILE__, __LINE__)


extern void  __uk_handler_string_contains( void *a, void *b, char *file, int line);
extern void  __uk_handler_string_does_not_contain( void *a, void *b, char *file, int line);

#define UKStringContains( a, b)       __uk_handler_string_contains( (a), (b), __FILE__, __LINE__)
#define UKStringDoesNotContain( a, b) __uk_handler_string_does_not_contain( (a), (b), __FILE__, __LINE__)


extern void  __uk_handler_does_not_raise_exception( void *exception, char *file, int line);
extern void  __uk_handler_raises_exception( void *exception, char *file, int line);
extern void  __uk_handler_raises_exception_class( void *exception, void *cls, char *file, int line);
extern void  __uk_handler_raises_exception_named( void *exception, void *name, char *file, int line);


#ifdef __OBJC__


#if HAVE_THROW 

#ifdef TRACE_INCLUDES_NS_DURING
# warning using @throw
#endif

#define UKRaisesExceptionClass( a, b)                           \
   do                                                           \
   {                                                            \
      id p_exp = nil;                                           \
                                                                \
      @try { a; }                                               \
      @catch( id exp) { p_exp = exp; }                          \
                                                                \
      __uk_handler_raises_exception( p_exp, [b class], __FILE__, __LINE__); \
   }                                                            \
   while( NO)

#else

#ifdef TRACE_INCLUDES_NS_DURING
# warning using NS_DURING
#endif

//
// this cant be done with backwards compatibility, since there
// the caught class is always NSException
//
// #define UKRaisesExceptionClass( a, b)                          
//
#endif


// common code: if HAVE_THROW, expect NS_DURING to be @throw

#define UKRaisesException( a)                                   \
   do                                                           \
   {                                                            \
      NSException   *exception;                                 \
                                                                \
      exception = nil;                                          \
                                                                \
      NS_DURING                                                 \
         a;                                                     \
      NS_HANDLER                                                \
         exception = localException;                            \
      NS_ENDHANDLER                                             \
                                                                \
      __uk_handler_raises_exception( exception, __FILE__, __LINE__); \
   }                                                            \
   while( NO)
   
#define UKDoesNotRaiseException( a)                             \
   do                                                           \
   {                                                            \
      NSException   *exception;                                 \
                                                                \
      exception = nil;                                          \
                                                                \
      NS_DURING                                                 \
         a;                                                     \
      NS_HANDLER                                                \
         exception = localException;                            \
      NS_ENDHANDLER                                             \
                                                                \
      __uk_handler_does_not_raise_exception( exception, __FILE__, __LINE__); \
   }                                                            \
   while( NO)

#define UKRaisesExceptionNamed( a, b)                           \
   do                                                           \
   {                                                            \
      NSException   *exception;                                 \
                                                                \
      exception = nil;                                          \
                                                                \
      NS_DURING                                                 \
         { a; }                                                 \
      NS_HANDLER                                                \
         exception = localException;                            \
      NS_ENDHANDLER                                             \
                                                                \
      __uk_handler_raises_exception( exception, (b), __FILE__, __LINE__); \
   }                                                            \
   while( NO)


#else

//
// c - exception code to be written here
// need some kind of setjmp rendezvous kind of code
//

#endif


extern int   __uk_run_test( void (*f)( void), char *file, int line);
extern int   __uk_run_test_in_pool( void (*f)( void), char *file, int line);
extern int   __uk_show_statistics( void);
extern void  __uk_reset_statistics( void);
extern int   __uk_set_quiet( int quiet);

#define UKRunTestFunction( f)         __uk_run_test( f, __FILE__, __LINE__)
#define UKRunTestFunctionInPool( f)   __uk_run_test_in_pool( f, __FILE__, __LINE__)
#define UKResetStatistics()           __uk_reset_statistics()
#define UKShowStatistics()            __uk_show_statistics()
#define UKSetQuiet( flag)             __uk_set_quiet( flag)

