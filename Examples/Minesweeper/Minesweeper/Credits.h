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

namespace Strings
{
	const char Programmer0[] PROGMEM = "Pharap";

	const char * const Programmers[] PROGMEM =
	{
		Programmer0
	};
	
	const char Artist0[] PROGMEM = "Vampirics";
	constexpr const char * Artist1 = Programmer0;

	const char * const Artists[] PROGMEM =
	{
		Artist0,
		Artist1
	};

	const char Tester0[] PROGMEM = "Noel Abercrombie";
	const char Tester1[] PROGMEM = "Filmote";

	const char * const Testers[] PROGMEM =
	{
		Tester0,
		Tester1
	};
}

#include "Languages.h"