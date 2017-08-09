/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2016, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 ******************************************************************************/


#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <errno.h>

#include "Jit.hpp"
#include "ilgen/TypeDictionary.hpp"
#include "ilgen/MethodBuilder.hpp"
#include "shifts.hpp"

using std::cout;
using std::cerr;
 
 // default is 64, see makefile for test for 32 (-D DO_32)
#ifdef DO_32
#define builderType Int32
#define cdeclType int32_t
#define MESSAGE "testing 32 bit shifts"
#else 
#define builderType Int64
#define cdeclType int64_t
#define MESSAGE "testing 64 bit shifts"
#endif

int
main(int argc, char *argv[])
   {
   cout << "Step 1: initialize JIT\n";
   bool initialized = initializeJit();
   if (!initialized)
      {
      cerr << "FAIL: could not initialize JIT\n";
      exit(-1);
      }

   cout << "Step 2: define type dictionary\n";
   TR::TypeDictionary types;

   cout << "Step 3: compile method builder\n";
   ShiftTests method(&types);
   uint8_t *entry = 0;
   int32_t rc = compileMethodBuilder(&method, &entry);
   if (rc != 0)
      {
      cerr << "FAIL: compilation error " << rc << "\n";
      exit(-2);
      }

   cout << "Step 4: invoke compiled code and print results\n";
   typedef cdeclType (ShiftTestsFunction)(cdeclType);
   ShiftTestsFunction *testShift = (ShiftTestsFunction *) entry;

   cout << MESSAGE;

   if (testShift (2) == 1) { 
      cout << "Step 4.1: Success:  shift 2 right by 1 and get 1\n";
   } else { 
      cout << "Step 4.1: Failed to shift 2 right by 1 and get 1\n";
      cout << testShift (2);
   }
   if (testShift (4) == 2) { 
      cout << "Step 4.2: Success:  shift 4 right by 1 and get 2\n";
   } else { 
      cout << "Step 4.2: Failed to shift 4 right by 1 and get 2\n";
      cout << testShift (4);
   }

   cout << "Step 5: shutdown JIT\n";
   shutdownJit();
   }



ShiftTests::ShiftTests(TR::TypeDictionary *d)
   : MethodBuilder(d)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   DefineName("testshift");
   DefineParameter("valueToShift", builderType);
   DefineReturnType(builderType);
   }

bool
ShiftTests::buildIL()
   {
   cout << "ShiftTests::buildIL() running!\n";

   Return(ShiftR(
            Load("valueToShift"), 
            ConvertTo(builderType, ConstInt32(1))));

   return true;
   }
