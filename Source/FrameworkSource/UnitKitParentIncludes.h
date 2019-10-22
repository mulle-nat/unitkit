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
#ifdef __OBJC__
# import <Foundation/Foundation.h>
#endif

#ifndef HAVE_THROW
# ifdef OBJC_API_VERSION
#  define HAVE_THROW (OBJC_API_VERSION >= 2)
# else
#  if defined( __GNU_LIBOBJC__) && __GNU_LIBOBJC__ >= 20100911 || defined( __MULLE_OBC__)
#   define HAVE_THROW  1
#  else
#   define HAVE_THROW  0
#  endif
# endif
#endif

