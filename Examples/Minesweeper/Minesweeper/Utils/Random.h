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

#include <Arduboy2.h>

inline unsigned long generateRandomSeed(void)
{
	/*
	power_adc_enable(); // ADC on

						// do an ADC read from an unconnected input pin
	ADCSRA |= _BV(ADSC); // start conversion (ADMUX has been pre-set in boot())
	while (bit_is_set(ADCSRA, ADSC)) {} // wait for conversion complete

	const unsigned long seed = (static_cast<unsigned long>(ADC) << 16) + micros();

	power_adc_disable(); // ADC off

	return seed;*/
	return rand()<< 16;
}
