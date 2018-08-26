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

template< typename True, typename False, bool Condition > struct Conditional;
template< typename True, typename False > struct Conditional<True, False, true> { using Type = True; };
template< typename True, typename False > struct Conditional<True, False, false> { using Type = False; };

template< typename True, typename False, bool Condition > using ConditionalType = typename Conditional<True, False, Condition>::Type;

template< typename True, typename False > using LargerType = ConditionalType<True, False, (sizeof(True) > sizeof(False))>;
template< typename True, typename False > using SmallerType = ConditionalType<True, False, (sizeof(True) < sizeof(False))>;