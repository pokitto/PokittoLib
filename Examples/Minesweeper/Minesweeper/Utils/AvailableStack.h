#pragma once

//
// Copyright (C) 2018 Pharap (@Pharap)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <stddef.h>
#include <stdint.h>

//
// This code is an improved version of the freeMemory function found at
// http://playground.arduino.cc/Code/AvailableMemory
//
// The original code makes no mention of authorship or copyright,
// and as of this file's creation neither repo has a licence.
//

inline ptrdiff_t getAvailableStackSpace(void)
{
	extern int __heap_start;
	extern int * __brkval; 
	
	const int stackTop = 0;
	(void)stackTop;
	
	return static_cast<ptrdiff_t>(&stackTop - ((__brkval == 0) ? &__heap_start : __brkval));
}