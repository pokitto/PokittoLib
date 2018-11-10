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

namespace Details
{
	template< typename T, T ... Ts>
	struct MaxValueHelper;

	template< typename T, T A, T B>
	struct MaxValueHelper<T, A, B>
	{
		constexpr const static T Value = (A > B) ? A : B;
	};

	template< typename T, T A, T B, T ... Ts>
	struct MaxValueHelper<T, A, B, Ts...> : MaxValueHelper<T, MaxValueHelper<T, A, B>::Value, Ts...>
	{
	};

	template< typename T, T ... Ts>
	struct MinValueHelper;

	template< typename T, T A, T B>
	struct MinValueHelper<T, A, B>
	{
		constexpr const static T Value = (A < B) ? A : B;
	};

	template< typename T, T A, T B, T ... Ts>
	struct MinValueHelper<T, A, B, Ts...> : MinValueHelper<T, MinValueHelper<T, A, B>::Value, Ts...>
	{
	};
}

template< typename T, T ... Ts >
constexpr T MaxValue(void)
{
	return Details::MaxValueHelper<T, Ts ...>::Value;
}

template< typename T, T ... Ts >
constexpr T MinValue(void)
{
	return Details::MinValueHelper<T, Ts ...>::Value;
}