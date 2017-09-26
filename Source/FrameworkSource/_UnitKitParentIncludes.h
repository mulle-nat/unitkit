/*
 This source is part of UnitKit, a unit test framework for Mac OS X
 development.

 Copyright (c) 2011 Mulle kybernetiK
 Coded by Nat!

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
#ifdef GNUSTEP
# import <GNUstepBase/GSObjCRuntime.h>  // ??
#else
# if defined(__MULLE_OBJC__)
#  import <mulle_objc_runtime/mulle_objc_runtime.h>
# else
#  if defined(__APPLE__)
#   import <objc/objc-runtime.h>
#  else
#   import <objc/runtime.h>
#  endif
# endif
#endif


#define UK_OBJC_RUNTIME_NEXT    0
#define UK_OBJC_RUNTIME_GNU1    1
#define UK_OBJC_RUNTIME_APPLE2  2
#define UK_OBJC_RUNTIME_GNU2    3

#if defined(__OBJC2__) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050
# define UK_IS_OBJC2_RUNTIME    (UK_OBJC_API_VERSION >= UK_OBJC_RUNTIME_APPLE)
#endif
#define UK_IS_APPLE_RUNTIME     (! UK_IS_GNU_RUNTIME)
#define UK_IS_GNU_RUNTIME       (UK_OBJC_API_VERSION & 1)

// define OBJC_API_VERSION
#ifdef __APPLE__
# include <objc/objc-api.h>
# define UK_OBJC_API_VERSION    OBJC_API_VERSION
#else
# if __GNU_LIBOBJC__ >= 20100911
#  define UK_OBJC_API_VERSION   UK_OBJC_RUNTIME_GNU2
# else
#  define UK_OBJC_API_VERSION   UK_OBJC_RUNTIME_GNU1
# endif
#endif
