/*
 This source is part of UnitKit, a unit test framework for Mac OS X
 development. You can find more information about UnitKit at:

 http://x180.net/Code/UnitKit

 Copyright (c)2004 James Duncan Davidson

 Contributions by Mark Dalrymple

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

#import "UKRunner.h"
#import "UKTest.h"
#import "UKTestHandler.h"


#define DEBUG_VERBOSE


#import "_UnitKitParentIncludes.h"

@interface NSBundle (APIAddedAfterTiger)
- (BOOL)preflightAndReturnError:(NSError **)error;
@end

@implementation UKRunner

+ (NSString *) localizedString:(NSString *)key
{
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    return NSLocalizedStringFromTableInBundle(key, @"UKRunner",
                                              bundle, @"");
}

+ (NSString *) displayStringForException:(id)exc
{
    if ([exc isKindOfClass:[NSException class]]) {
        return [NSString stringWithFormat:@"NSException: %@ %@", [exc name],
            [exc reason]];
    } else {
        return NSStringFromClass([exc class]);
    }
}


- (int) showStatistics
{
   int        testsPassed;
   int        testsFailed;
   int        testClasses;
   int        testMethods;

   testsPassed = [[UKTestHandler handler] testsPassed];
   testsFailed = [[UKTestHandler handler] testsFailed];
   testClasses = testClassesRun;
   testMethods = testMethodsRun;


   // XXX i18n
   printf("Result: %i classes, %i methods, %i tests, %i failed\n", testClasses, testMethods, (testsPassed + testsFailed), testsFailed);
   return( testsFailed == 0 ? 0 : -1);
}



+ (int) runTestsWithRunner:(UKRunner *) runner
               bundleNames:(NSArray *) bundleNames
                classNames:(NSArray *) classNames

{
   NSBundle   *testBundle;
   NSSet      *set;
   NSString   *cwd;
   NSString   *bundlePath;
   int        argCount;
   int        i;
    /*
    We expect the following usage:
    $ ukrun [BundleName]

    If there are no arguments given, then we'll just execute every
    test class found. Otherwise...
    */

   cwd      = [[NSFileManager defaultManager] currentDirectoryPath];
   argCount = [bundleNames count];

   for( i = 0; i < argCount; i++)
   {
      bundlePath = [bundleNames objectAtIndex:i];

#ifdef DEBUG_VERBOSE
      fprintf( stderr, "bundle path: \"%s\"\n", [bundlePath cString]);
#endif
      bundlePath = [bundlePath stringByExpandingTildeInPath];
      if ( ![ bundlePath isAbsolutePath])
      {
         bundlePath = [cwd stringByAppendingPathComponent:bundlePath];
         bundlePath = [bundlePath stringByStandardizingPath];
      }

#ifdef DEBUG_VERBOSE
      fprintf( stderr, "looking for bundle at path: \"%s\"\n", [bundlePath cString]);
#endif
      // make sure bundle exists and is loaded

      testBundle = [NSBundle bundleWithPath:bundlePath];

      if (testBundle == nil)
      {
         // XXX i18n as well as message improvements
         printf("Test bundle \"%s\" not found\n", [bundlePath cString]);
         return -1;
      }

      if (testBundle && [testBundle respondsToSelector:@selector(preflightAndReturnError:)])
      {
        NSError *error = nil;
        BOOL willLoad = [testBundle preflightAndReturnError:&error];

        if ( error && ! willLoad) {
            printf("Test bundle \"%s\" exists, but will not load: %s\n", [bundlePath cString], [[error description] cString]);
        }
      }

#ifdef __MULLE_OBJC__
      if( ! [testBundle loadBundle])
#else
      if( ! [testBundle load])
#endif
      {
         NSString   *reason;

         reason = nil;
         // XXX i18n as well as message improvements
         if( [testBundle respondsToSelector:@selector( _loadFailureReason)])
            reason = [testBundle performSelector:@selector( _loadFailureReason)];
         if( ! reason)
            reason = @"unknown reason";

         printf("Test bundle \"%s\" exists, but could not be loaded: %s\n", [bundlePath cString], [reason cString]);
         return -1;
      }

      set = nil;
      if( classNames)
         set = [NSSet setWithArray:classNames];
      [runner runTestsInBundle:testBundle
                  classNames:set];
   }

   return( [runner showStatistics]);
}


- (void) setDontIgnoreUnexpectedExceptions:(BOOL) flag
{
   dontIgnoreUnexpectedExceptions = flag;
}


- (void) setDontUseRunLoop:(BOOL) flag
{
   dontUseRunLoop = flag;
}


static int  gobble_switch( NSMutableArray *arguments, NSString *s)
{
   unsigned long  i;

   i = [arguments indexOfObject:s];
   if( i == NSNotFound)
      return( 0);
   [arguments removeObjectAtIndex:i];
   return( 1);
}


static int  gobble_test( NSMutableArray *arguments, NSString **name)
{
   unsigned long  i;

   i = [arguments indexOfObject:@"-test"];
   if( i == NSNotFound)
      return( 0);

   [arguments removeObjectAtIndex:i];
   if( i >= [arguments count])
      return( -1);

   *name = [[[arguments objectAtIndex:i] retain] autorelease];
   [arguments removeObjectAtIndex:i];
   return( 1);
}


static void  bail_out( NSException *exception)
{
   NSLog( @"uncaught exception: %@", exception);
   exit( 1);
}


+ (int) runTests
{
   NSMutableArray   *classNames;
   NSString         *className;
   UKRunner         *runner;
   int              rval;
   id               args;
   BOOL             noIgnore;
   BOOL             noRunLoop;

   noIgnore  = NO;
   noRunLoop = NO;

   args = [[NSProcessInfo processInfo] arguments];
   args = [[args mutableCopy] autorelease];

   /* paranoid check */
   if( ! [args count])
      goto usage;

   // remove /bin/ukrun
   [args removeObjectAtIndex:0];

//   if( gobble_switch( args, @"-v"))
//      printf( "ukrun version " UNITKIT_VERSION "\n");

   // Mark Dalrymple contributed this bit about going quiet.
   if( gobble_switch( args, @"-q"))
      [[UKTestHandler handler] setQuiet:YES];

   // if something fails, immediately raise and abort
   if( gobble_switch( args, @"-e"))
   {
      [[UKTestHandler handler] setExitOnFailure:YES];
      NSSetUncaughtExceptionHandler( (void (*)( void *)) bail_out);
   }

   // obscure stacktraces ? then use this
   if( gobble_switch( args, @"-r"))
      noRunLoop = YES;

   if( gobble_switch( args, @"-E"))
      noIgnore = YES;

   classNames = nil;
   for(;;)
   {
      switch( gobble_test( args, &className))
      {
      case -1 : goto usage;
      case  1 : if( ! classNames)
                   classNames = [NSMutableArray array];
                [classNames addObject:className];
                continue;
      }
      break;
   }

   if( ! [args count])
      goto usage;

   @autoreleasepool
   {
      runner = [[UKRunner new] autorelease];
      [runner setDontIgnoreUnexpectedExceptions:noIgnore];
      [runner setDontUseRunLoop:noRunLoop];

      rval   = [self runTestsWithRunner:runner
                            bundleNames:args
                             classNames:classNames];
   }
   return( rval);

usage:
   printf("Usage: ukrun [-eEqv] [-test <class>::<method>]* [bundle]+\n");

   return( -2);
}


- (void) runLoopBouncyBounce
{
#ifdef DEBUG_RUNLOOP
   NSLog( @"-[%@ %@]", [__currentTarget class], NSStringFromSelector( currentSelector));
#endif
   [__currentTarget performSelector:currentSelector];
   runLoopDone = YES;
}


- (void) runTest:(SEL) testSelector
        onObject:(id) testObject
{
    NSRunLoop   *runLoop;

    if( dontUseRunLoop)
    {
       [testObject performSelector:testSelector];
       return;
    }

    runLoop = [NSRunLoop currentRunLoop];

    NSCParameterAssert( runLoop);

    __currentTarget   = testObject;
    currentSelector = testSelector;
    runLoopDone     = NO;

    [runLoop performSelector:@selector( runLoopBouncyBounce)
                      target:self
                    argument:nil
                       order:ULONG_MAX
                       modes:[NSArray arrayWithObject:NSDefaultRunLoopMode]];

#ifdef __COREFOUNDATION__
    for(;;)
    {
        switch( CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0.0, 1))
        {
        case kCFRunLoopRunTimedOut :
        case kCFRunLoopRunFinished : return;
        }
    }
#else
    while( ! runLoopDone)
        [runLoop runUntilDate:nil];
#endif
}

/*!
 @method runTests:onClass:
 @param testMethods An array containing the list of method names to execute on the test class.
 @param testClass The class on which to perform the test methods on
 @abstract Runs a set of tests on instances of the given class
 @discussion This method takes a class and a list of methods that should be executed on it. For each method in the list, an object instance of the class will be created and the method called on it. If there is a problem with the instanation of the class, or in the release of that object instance, an error will be reported and all test execution on the class will end. If there is an error while running the test method, an error will be reported and execution will move on to the next method.
 */

- (void) runTests:(NSArray *)testMethods
          onClass:(Class)testClass
{
   NSAutoreleasePool   *pool;
   NSEnumerator        *e;
   NSString            *msg;
   NSString            *testMethodName;
   SEL                 testSel;
   id                  testObject;

   /*
    The hairy thing about this method is catching and dealing with all of
    the permutations of uncaught exceptions that might be heading our way.
    */

   e = [testMethods objectEnumerator];
   while( testMethodName = [e nextObject])
   {
      testMethodsRun++;

      pool = [NSAutoreleasePool new];

   NS_DURING
      testObject = [testClass new];
   NS_HANDLER
      msg = [UKRunner localizedString:@"errExceptionOnInit"];
      msg = [NSString stringWithFormat:msg, NSStringFromClass(testClass), [localException name]];
      [[UKTestHandler handler] reportWarning:msg];
      [pool release];
      return;
   NS_ENDHANDLER

   NS_DURING
#ifdef DEBUG_VERBOSE
      fprintf( stderr, "testing -[%s %s]\n", [NSStringFromClass( [testObject class]) cString], [testMethodName cString]);
#endif
      testSel = NSSelectorFromString(testMethodName);
      [self runTest:testSel
           onObject:testObject];
   NS_HANDLER
      msg = [UKRunner localizedString:@"errExceptionInTestMethod"];
      msg = [NSString stringWithFormat:msg, NSStringFromClass(testClass), testMethodName, [localException name]];
      [[UKTestHandler handler] reportWarning:msg];

      //
      // UnitKit let it slide, by default. But is this good ?
      // do -e on ukrun to enable this
      //
      if( dontIgnoreUnexpectedExceptions)
      {
         [pool release];
         exit( 1);
      }
   NS_ENDHANDLER

   NS_DURING
      [testObject release];
   NS_HANDLER
      msg = [UKRunner localizedString:@"errExceptionOnRelease"];
      msg = [NSString stringWithFormat:msg, NSStringFromClass(testClass), [localException name]];
      [[UKTestHandler handler] reportWarning:msg];
      [pool release];
      return;
   NS_ENDHANDLER

      [pool release];
   }
}


- (void) runTestsInClass:(Class)testClass
{
    NSArray   *testMethods;

    testClassesRun++;

    testMethods = UKTestMethodNamesFromClass( testClass);
    [self runTests:testMethods
           onClass:testClass];
}


- (void) runTestsInBundle:(NSBundle *) bundle
               classNames:(NSSet *) classNames
{
   NSArray         *testClasses;
   NSEnumerator    *e;
   NSString        *testClassName;

   testClasses = UKTestClassNamesFromBundle(bundle);

   e = [testClasses objectEnumerator];
   while( testClassName = [e nextObject])
      if( ! classNames || [classNames containsObject:testClassName])
         [self runTestsInClass:NSClassFromString( testClassName)];
}


- (void) runTestsInBundle:(NSBundle *)bundle
{
   [self runTestsInBundle:bundle
               classNames:nil];
}

@end


#if UK_OBJC_API_VERSION != UK_OBJC_RUNTIME_GNU1
static BOOL  isSubclass( Class c, Class parent)
{
   while( c)
   {
      if( c == parent)
         return( YES);
#if UK_IS_OBJC2_RUNTIME
      c = class_getSuperclass( c);
#else
      c = ((struct objc_class *) c)->super_class;
#endif
   }
   return( NO);
}
#endif


NSArray *UKTestClassNamesFromBundle(NSBundle *bundle)
{
    NSMutableArray *testClassNames;

   testClassNames = [NSMutableArray array];

#if UK_OBJC_API_VERSION != UK_OBJC_RUNTIME_GNU1
   BOOL   flag;

    /*
     I found the code to walk the classes in the system from an example in
     Apple's documentation. Pretty much all I changed was the bit that tested
     which bundle a class came from and that it implements the UKTest protocol.
     It's a bit low level (hell, there's a malloc and free here!), but there
     doesn't seem to be any easier way to get a list of classes from a bundle.

     I keep thinking that this kind of functionality could be factored out
     into a general "What classes are in which bundles and what classes
     implement what protocols. But so far I've resisted the urge.
     */

   int numClasses;
   numClasses = objc_getClassList(NULL, 0);
   if (numClasses > 0) {
      Class *classes = malloc(sizeof(Class) * numClasses);
      (void) objc_getClassList (classes, numClasses);
      int i;
      for (i = 0; i < numClasses; i++) {
         Class c = classes[i];
         NSBundle *classBundle = [NSBundle bundleForClass:c];
         if( bundle != classBundle)
            continue;
         flag = isSubclass( c, [NSProxy class]); //NSProxy doesn't
         if( flag)
            continue;
         flag = isSubclass( c, [NSObject class]); //NSProxy doesn't
         if( ! flag)
            continue;
         if( ! [c conformsToProtocol:@protocol( UKTest)])
            continue;
         [testClassNames addObject:NSStringFromClass(c)];
      }
      free(classes);
   }

#else

    /*
     Nicolas Roard contributed the following code to pick up test classes
     from a bundle.
     */

    Class class;
    void *es = NULL;
    while ((class = objc_next_class (&es)) != Nil)
    {
        NSBundle* classBundle = [NSBundle bundleForClass: class];
        if (bundle == classBundle &&
            [class conformsToProtocol: @protocol (UKTest)]) {
            [testClassNames addObject:NSStringFromClass(class)];
        }
    }

#endif
   [testClassNames sortUsingSelector:@selector(caseInsensitiveCompare:)];
   return( testClassNames);
}


NSArray   *UKTestMethodNamesFromClass(Class c)
{

   NSMutableArray *testMethods;

   testMethods = [NSMutableArray array];

#if UK_OBJC_API_VERSION != UK_OBJC_RUNTIME_GNU1

#ifdef UK_IS_OBJC2_RUNTIME
   {
      Method        *methods;
      Method        method;
      NSString      *methodName;
      unsigned int  i, n;
      SEL           sel;

      n = 0; /* paranoia */
      methods = class_copyMethodList( c, &n);
      for( i = 0; i < n; i++)
      {
         method     = methods[ i];
         sel        = method_getName( method);
         methodName = NSStringFromSelector( sel);

         if ([methodName hasPrefix:@"test"])
            [testMethods addObject:methodName];
      }
      free( methods);
   }
#else
   /*
    I think I picked this code up originaly from some Apple sample code. But
    I could be wrong. It's landed here from all of my previous UnitKit
    iterations. Of course, it's been modified to look for methods that start
    with a test prefix.
    */
   void *iterator = 0;
   struct objc_method_list *mlist = class_nextMethodList(c, &iterator);
   while (mlist != NULL) {
      int i;
      for (i = 0; i < mlist->method_count; i++) {
         Method method = &(mlist->method_list[i]);
         if (method == NULL) {
            continue;
         }
         SEL sel = method->method_name;
         NSString *methodName = NSStringFromSelector(sel);
         if ([methodName hasPrefix:@"test"]) {
            [testMethods addObject:methodName];
         }
      }
      mlist = class_nextMethodList(c, &iterator);
   }
#endif

#else

   /*
    Nicolas Roard contributed the following code to pick up test classes
    from a bundle.
    */

   MethodList_t _methods = c->methods;
   int i;
   for (i=0; i < _methods->method_count; i++) {
      Method_t method = &(_methods->method_list[i]);
      if (method == NULL) {
         continue;
      }
      SEL sel = method->method_name;
      NSString *methodName = NSStringFromSelector(sel);
      if ([methodName hasPrefix:@"test"]) {
         [testMethods addObject:methodName];
      }
   }

#endif

   [testMethods sortUsingSelector:@selector( caseInsensitiveCompare:)];
   return( testMethods);
}
