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

#include <stdint.h>

template< typename THash >
struct Checksum
{
	using HashType = THash;
	using ResultType = decltype(HashType::hash(0));

	static ResultType calculateChecksum(const char block[], size_t size)
	{
		constexpr size_t resultSize = sizeof(ResultType);

		ResultType sum = static_cast<ResultType>(size);
		for (size_t i = 0; i < size; i += resultSize)
		{
			ResultType value = 0;
			for (uint8_t j = 0; j < resultSize; ++j)
			{
				value <<= 8;
				
				const size_t index = (i + j);
				if(index < size)
					value |= block[index];
			}
			sum = HashType::hash(sum + value);
		}
		return sum;
	}

	template< size_t size >
	static ResultType calculateChecksum(const char (&block)[size])
	{
		return calculateChecksum(block, size);
	}

	template< typename T >
	static ResultType calculateChecksum(const T & data)
	{
		return calculateChecksum(reinterpret_cast<const char *>(&data), sizeof(T));
	}
};