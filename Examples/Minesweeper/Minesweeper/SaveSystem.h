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

#include "Utils.h"

template< typename TChecksum, size_t EeepromCapacityValue, size_t SaveStartValue, typename TData >
class SaveSystem
{
public:
	SaveSystem(void) = delete;

public:
	using ChecksumType = TChecksum;
	using DataType = TData;

	struct Header
	{
		uint32_t checksum;
		uint16_t dataSize;
	};

	struct SaveBlock
	{
		Header header;
		DataType data;
	};

public:
	static constexpr size_t EepromCapacity = EeepromCapacityValue;

	static constexpr size_t SaveStart = SaveStartValue;
	static constexpr size_t SaveSize = sizeof(SaveBlock);

	static constexpr size_t HeaderStart = SaveStart;
	static constexpr size_t HeaderSize = sizeof(Header);

	static constexpr size_t ChecksumStart = SaveStart;
	static constexpr size_t ChecksumSize = sizeof(uint32_t);

	static constexpr size_t DataSizeStart = ChecksumStart + ChecksumSize;
	static constexpr size_t DataSizeSize = sizeof(uint16_t);

	static constexpr size_t DataStart = DataSizeStart + DataSizeSize;
	static constexpr size_t DataSize = sizeof(DataType);

	static_assert(SaveSize <= EepromCapacity, "Save size is larger than the EEPROM capacity");
	static_assert(SaveStart + SaveSize <= EepromCapacity, "The save size can't fit at the provided offset");

private:
	static constexpr SaveBlock * SaveAddress = reinterpret_cast<SaveBlock *>(SaveStart);
	static constexpr Header * HeaderAddress = reinterpret_cast<Header *>(HeaderStart);
	static constexpr DataType * DataAddress = reinterpret_cast<DataType *>(DataStart);

	static constexpr uint32_t * ChecksumAddress = reinterpret_cast<uint32_t *>(ChecksumStart);
	static constexpr uint16_t * DataSizeAddress = reinterpret_cast<uint16_t *>(DataSizeStart);

public:
	// Initialise the save data when there's no save file
	static void initialise(const DataType & data)
	{
		SaveBlock saveBlock;

		saveBlock.data = data;
		saveBlock.header.dataSize = DataSize;
		saveBlock.header.checksum = ChecksumType::calculateChecksum(saveBlock.data);

		//Eeprom::update(SaveAddress, saveBlock);
	}

	// Calculate the checksum from the data in EEPROM
	static uint32_t calculateChecksum(void)
	{
		/*
		const size_t dataSize = static_cast<size_t>(Eeprom::read(DataSizeAddress));

		const ptrdiff_t availableStackSpace = getAvailableStackSpace();
		if(availableStackSpace < 0 || dataSize >= static_cast<size_t>(availableStackSpace))
			return 0;

		char buffer[dataSize];
		Eeprom::read(DataStart, buffer, dataSize);

		return ChecksumType::calculateChecksum(buffer, dataSize);
		*/
	}

	// Verify the file checksum
	static bool verifyChecksum(void)
	{
		// Used by checksum and savedChecksum
		constexpr size_t usedSpace = sizeof(uint32_t) * 2;

		const size_t dataSize = 0; //static_cast<size_t>(Eeprom::read(DataSizeAddress));

		const ptrdiff_t availableStackSpace = getAvailableStackSpace();
		if(availableStackSpace < 0 || dataSize >= static_cast<size_t>(availableStackSpace - usedSpace))
			return false;

		char buffer[dataSize];
		//Eeprom::read(DataStart, buffer, dataSize);

		const uint32_t checksum = ChecksumType::calculateChecksum(buffer, dataSize);
		const uint32_t savedChecksum = 0; //Eeprom::read(ChecksumAddress);

		return (savedChecksum == checksum);
	}

	// Load the data
	static DataType loadData(void)
	{
		const uint16_t dataSize = 0;// Eeprom::read(DataSizeAddress);
		const size_t limit = (dataSize < DataSize) ? dataSize : DataSize;

		DataType data;
		char * dataBuffer = reinterpret_cast<char *>(&data);
		//Eeprom::read(reinterpret_cast<const char *>(DataAddress), dataBuffer, limit);
		return data;
	}

	// Save the data block and update the checksum
	// using the data size from the save file
	static void saveData(const DataType & data)
	{
		//Eeprom::update(DataAddress, data);

		uint16_t dataSize = 0;//Eeprom::read(DataSizeAddress);
		if(dataSize < DataSize)
		{
			dataSize = DataSize;
			//Eeprom::update(DataSizeAddress, DataSize);
		}

		// Used by checksum
		constexpr size_t usedSpace = sizeof(uint32_t);

		const ptrdiff_t availableStackSpace = getAvailableStackSpace();
		if(availableStackSpace < 0 || dataSize >= static_cast<size_t>(availableStackSpace - usedSpace))
			return;

		char buffer[dataSize];
		//Eeprom::read(DataStart, buffer, dataSize);

		const uint32_t checksum = ChecksumType::calculateChecksum(buffer, dataSize);

		//Eeprom::update(ChecksumAddress, checksum);
	}
};
