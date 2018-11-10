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

#define BEGIN_LANGUAGE_VARIANT(language, location) \
namespace Languages \
{ \
namespace language \
{ \
namespace location \
{

#define END_LANGUAGE_VARIANT \
} \
} \
}

#define STRING(name, value) const char name [] PROGMEM = value ;

// Titlescreen
#define STRING_PLAY(text) STRING(Play, text)
#define STRING_CREDITS(text) STRING(Credits, text)
#define STRING_STATS(text) STRING(Stats, text)
#define STRING_SETTINGS(text) STRING(Settings, text)
#define STRING_THEMES(text) STRING(Themes, text)

// Game
#define STRING_YOU_WIN(text) STRING(YouWin, text)
#define STRING_YOU_LOSE(text) STRING(YouLose, text)
#define STRING_PLAY_AGAIN(text) STRING(PlayAgain, text)
#define STRING_YES(text) STRING(Yes, text)
#define STRING_NO(text) STRING(No, text)

// Themes
#define STRING_THEME_SEA(text) STRING(Sea, text)
#define STRING_THEME_LAND(text) STRING(Land, text)
#define STRING_THEME_CACTUS(text) STRING(Cactus, text)
#define STRING_THEME_TURTLE(text) STRING(Turtle, text)
#define STRING_THEM_PENGUIN(text) STRING(Penguin, text)

// Credits
#define STRING_PROGRAMMING(text) STRING(Programming, text)
#define STRING_GRAPHICS(text) STRING(Graphics, text)
#define STRING_TESTING(text) STRING(Testing, text)
#define STRING_LOCALISATION(text) STRING(Localisation, text)

// Stats
#define STRING_WINS(text) STRING(Wins, text)
#define STRING_LOSSES(text) STRING(Losses, text)
#define STRING_DELETE(text) STRING(Delete, text)
#define STRING_DELETED(text) STRING(Deleted, text)

// SaveCheck
#define STRING_CREATE_SAVE(text) STRING(CreateSave, text)

// Translators
#define STRING_TRANSLATOR(index, name) STRING( Translator##index, name)
#define BEGIN_TRANSLATORS \
	const char * const Translators[] PROGMEM = \
	{
#define TRANSLATOR(index) Translator##index,
#define END_TRANSLATORS };

#include "EN-GB.h"
#include "EN-AU.h"
#include "EN-LCY.h"
#include "FR-FR.h"
#include "ES-CL.h"
#include "DE-DE.h"

#undef BEGIN_LANGUAGE_VARIANT
#undef END_LANGUAGE_VARIANT

#undef STRING

// Titlescreen
#undef STRING_PLAY
#undef STRING_CREDITS
#undef STRING_STATS
#undef STRING_SETTINGS
#undef STRING_THEMES

// Game
#undef STRING_YOU_WIN
#undef STRING_YOU_LOSE
#undef STRING_PLAY_AGAIN
#undef STRING_YES
#undef STRING_NO

// Credits
#undef STRING_PROGRAMMING
#undef STRING_GRAPHICS
#undef STRING_TESTING
#undef STRING_LOCALISATION

// Stats
#undef STRING_WINS
#undef STRING_LOSSES
#undef STRING_DELETE
#undef STRING_DELETING
#undef STRING_DELETED

// SaveCheck
#undef STRING_CREATE_SAVE

// Translators
#undef STRING_TRANSLATOR
#undef BEGIN_TRANSLATORS
#undef TRANSLATOR
#undef END_TRANSLATORS

namespace Strings
{
	#if defined(LANGUAGE_EN_GB) || defined(LANGUAGE_EN)
		using namespace Languages::EN::GB;
	#elif defined(LANGUAGE_EN_LCY)
		using namespace Languages::EN::LCY;
	#elif defined(LANGUAGE_EN_AU)
		using namespace Languages::EN::AU;
	#elif defined(LANGUAGE_FR_FR) || defined(LANGUAGE_FR)
		using namespace Languages::FR::FR;
	#elif defined(LANGUAGE_DE_DE) || defined(LANGUAGE_DE)
		using namespace Languages::DE::DE;
	#elif defined(LANGUAGE_ES_CL) || defined(LANGUAGE_ES)
		using namespace Languages::ES::CL;
	#else
		using namespace Languages::EN::GB;
		#warning "No language defined, defaulting to EN-GB."
	#endif

	const char ColonSpace[] PROGMEM = ": ";
	const char Selected[] PROGMEM = "> ";
	const char NotSelected[] PROGMEM = "  ";

	char const * const ThemeNames[] PROGMEM =
	{
		Sea,
		Land,
		Turtle,
		Cactus,
		Penguin,
	};
}