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
//#include <avr/eeprom.h>

namespace Eeprom
{
	//
	// write
	//

	inline void write(uintptr_t address, uint8_t data)
	{
		eeprom_write_byte(reinterpret_cast<uint8_t *>(address), data);
	}

	inline void write(uintptr_t address, uint16_t data)
	{
		eeprom_write_word(reinterpret_cast<uint16_t *>(address), data);
	}

	inline void write(uintptr_t address, uint32_t data)
	{
		eeprom_write_dword(reinterpret_cast<uint32_t *>(address), data);
	}

	inline void write(uintptr_t address, float data)
	{
		eeprom_write_float(reinterpret_cast<float *>(address), data);
	}

	template< typename T >
	void write(uintptr_t address, const T & object)
	{
		eeprom_write_block(&object, reinterpret_cast<void *>(address), sizeof(T));
	}

	template< typename T, size_t size >
	void write(uintptr_t address, const T (&array)[size])
	{
		eeprom_write_block(&array[0], reinterpret_cast<void *>(address), sizeof(T) * size);
	}

	template< typename T >
	void write(uintptr_t address, const T array[], size_t quantity)
	{
		eeprom_write_block(&array[0], reinterpret_cast<void *>(address), sizeof(T) * quantity);
	}

	inline void write(uint8_t * address, uint8_t data)
	{
		eeprom_write_byte(address, data);
	}

	inline void write(uint16_t * address, uint16_t data)
	{
		eeprom_write_word(address, data);
	}

	inline void write(uint32_t *  address, uint32_t data)
	{
		eeprom_write_dword(address, data);
	}

	inline void write(float * address, float data)
	{
		eeprom_write_float(address, data);
	}

	template< typename T >
	void write(T * address, const T & object)
	{
		eeprom_write_block(&object, address, sizeof(T));
	}

	template< typename T, size_t size >
	void write(T * address, T (&array)[size])
	{
		eeprom_write_block(&array[0], address, sizeof(T) * size);
	}

	template< typename T >
	void write(T * address, const T array[], size_t quantity)
	{
		eeprom_write_block(&array[0], address, sizeof(T) * quantity);
	}

	//
	// update
	//

	inline void update(uintptr_t address, uint8_t data)
	{
		eeprom_update_byte(reinterpret_cast<uint8_t *>(address), data);
	}

	inline void update(uintptr_t address, uint16_t data)
	{
		eeprom_update_word(reinterpret_cast<uint16_t *>(address), data);
	}

	inline void update(uintptr_t address, uint32_t data)
	{
		eeprom_update_dword(reinterpret_cast<uint32_t *>(address), data);
	}

	inline void update(uintptr_t address, float data)
	{
		eeprom_update_float(reinterpret_cast<float *>(address), data);
	}

	template< typename T >
	void update(uintptr_t address, const T & object)
	{
		eeprom_update_block(&object, reinterpret_cast<void *>(address), sizeof(T));
	}

	template< typename T, size_t size >
	void update(uintptr_t address, const T (&array)[size])
	{
		eeprom_update_block(&array[0], reinterpret_cast<void *>(address), sizeof(T) * size);
	}

	template< typename T >
	void update(uintptr_t address, const T array[], size_t quantity)
	{
		eeprom_update_block(&array[0], reinterpret_cast<void *>(address), sizeof(T) * quantity);
	}

	inline void update(uint8_t * address, uint8_t data)
	{
		eeprom_update_byte(address, data);
	}

	inline void update(uint16_t * address, uint16_t data)
	{
		eeprom_update_word(address, data);
	}

	inline void update(uint32_t *  address, uint32_t data)
	{
		eeprom_update_dword(address, data);
	}

	inline void update(float * address, float data)
	{
		eeprom_update_float(address, data);
	}

	template< typename T >
	void update(T * address, const T & object)
	{
		eeprom_update_block(&object, address, sizeof(T));
	}

	template< typename T, size_t size >
	void update(T * address, T (&array)[size])
	{
		eeprom_update_block(&array[0], address, sizeof(T) * size);
	}

	template< typename T >
	void update(T * address, const T array[], size_t quantity)
	{
		eeprom_update_block(&array[0], address, sizeof(T) * quantity);
	}

	//
	// read
	//

	inline void read(uintptr_t address, uint8_t & data)
	{
		data = eeprom_read_byte(reinterpret_cast<const uint8_t *>(address));
	}

	inline void read(uintptr_t address, uint16_t & data)
	{
		data = eeprom_read_word(reinterpret_cast<const uint16_t *>(address));
	}

	inline void read(uintptr_t address, uint32_t & data)
	{
		data = eeprom_read_dword(reinterpret_cast<const uint32_t *>(address));
	}

	inline void read(uintptr_t address, float & data)
	{
		data = eeprom_read_float(reinterpret_cast<const float *>(address));
	}

	template< typename T >
	void read(uintptr_t address, T & object)
	{
		eeprom_read_block(&object, reinterpret_cast<const void *>(address), sizeof(T));
	}

	template< typename T, size_t size >
	void read(uintptr_t address, T (&array)[size])
	{
		eeprom_read_block(&array[0], reinterpret_cast<const void *>(address), sizeof(T) * size);
	}

	template< typename T >
	void read(uintptr_t address, T array[], size_t quantity)
	{
		eeprom_read_block(&array[0], reinterpret_cast<const void *>(address), sizeof(T) * quantity);
	}

	template< typename T >
	void read(const T * address, T & object)
	{
		eeprom_read_block(&object, address, sizeof(T));
	}

	template< typename T, size_t size >
	void read(const T * address, T (&array)[size])
	{
		eeprom_read_block(&array[0], address, sizeof(T) * size);
	}

	template< typename T >
	void read(const T * address, T array[], size_t quantity)
	{
		eeprom_read_block(&array[0], address, sizeof(T) * quantity);
	}

	template< typename T >
	T read(uintptr_t address)
	{
		T object;
		eeprom_read_block(&object, reinterpret_cast<const T *>(address), sizeof(T));
		return object;
	}

	template<>
	inline uint8_t read<uint8_t>(uintptr_t address)
	{
		return eeprom_read_byte(reinterpret_cast<const uint8_t *>(address));
	}

	template<>
	inline uint16_t read<uint16_t>(uintptr_t address)
	{
		return eeprom_read_word(reinterpret_cast<const uint16_t *>(address));
	}

	template<>
	inline uint32_t read<uint32_t>(uintptr_t address)
	{
		return eeprom_read_dword(reinterpret_cast<const uint32_t *>(address));
	}

	template<>
	inline float read<float>(uintptr_t address)
	{
		return eeprom_read_float(reinterpret_cast<const float *>(address));
	}

	template< typename T >
	T read(const T * address)
	{
		T object;
		eeprom_read_block(&object, address, sizeof(T));
		return object;
	}

	inline uint8_t read(const uint8_t * address)
	{
		return eeprom_read_byte(address);
	}

	inline uint16_t read(const uint16_t * address)
	{
		return eeprom_read_word(address);
	}

	inline uint32_t read(const uint32_t * address)
	{
		return eeprom_read_dword(address);
	}

	inline float read(const float * address)
	{
		return eeprom_read_float(address);
	}
}
