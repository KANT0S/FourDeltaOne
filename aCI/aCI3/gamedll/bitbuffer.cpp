// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Bitstream class.
//
// Initial author: NTAuthority
// Started: 2013-02-14
// ==========================================================

#include "aci3.h"
#include "bitbuffer.h"

namespace ci
{
BitBuffer::BitBuffer()
{
	init((char*)halloc(1024), 1024);

	_bytesManaged = true;
}

BitBuffer::BitBuffer(char* bytes, int length)
{
	init(bytes, length);
}

BitBuffer::~BitBuffer()
{
	if (_bytesManaged)
	{
		hfree(_bytes);
	}
}

void BitBuffer::init(char* bytes, int length)
{
	_bytes = (BYTE*)bytes;
	_curBit = 0;
	_maxBit = length * 8;
	_useDataTypes = true;
}

bool BitBuffer::write(int bits, const void* data)
{
	if (bits == 0)
	{
		return false;
	}

	// actually copied from disassembly, blah
	int bit = bits;

	BYTE* myData = (BYTE*)data;

	if ((_curBit + (bits)) > _maxBit)
	{
		_bytes = (BYTE*)hrealloc(_bytes, (_maxBit / 8) + 32);
		_maxBit = (_maxBit) + (32 * 8);
	}

	while (bit > 0)
	{
		int bitPos = _curBit & 7;
		int remBit = 8 - bitPos;
		int thisWrite = (bit < remBit) ? bit : remBit;

		BYTE mask = ((0xFF >> remBit) | (0xFF << (bitPos + thisWrite)));
		int bytePos = _curBit >> 3;

		BYTE tempByte = (mask & _bytes[bytePos]);
		BYTE thisBit = ((bits - bit) & 7);
		int thisByte = (bits - bit) >> 3;

		BYTE thisData = myData[thisByte];

		int nextByte = (((bits - 1) >> 3) > thisByte) ? myData[thisByte + 1] : 0;

		thisData = ((nextByte << (8 - thisBit)) | (thisData >> thisBit));

		BYTE outByte = (~mask & (thisData << bitPos) | tempByte);
		_bytes[bytePos] = outByte;

		_curBit += thisWrite;
		bit -= thisWrite;

		/*if (_maxBit < _curBit)
		{
			_maxBit = _curBit;
		}*/
	}

	return true;
}

bool BitBuffer::read(int bits, void* output)
{
	if (bits == 0)
	{
		return false;
	}

	if ((_curBit + bits) > _maxBit)
	{
		return false;
	}

	int curByte = _curBit >> 3;
	int curOut = 0;

	BYTE* outputStuff = (BYTE*)output;

	while (bits > 0)
	{
		int minBit = (bits < 8) ? bits : 8;

		int thisByte = (int)_bytes[curByte];
		curByte++;

		int remain = _curBit & 7;

		if ((minBit + remain) <= 8)
		{
			outputStuff[curOut] = (BYTE)((0xFF >> (8 - minBit)) & (thisByte >> remain));
		}
		else
		{
			outputStuff[curOut] = (BYTE)((0xFF >> (8 - minBit)) & (_bytes[curByte] << (8 - remain)) | (thisByte >> remain));
		}

		curOut++;
		_curBit += minBit;
		bits -= minBit;
	}

	return true;
}

bool BitBuffer::writeBoolean(bool data)
{
	if (writeDataType(1))
	{
		return write(1, &data);
	}

	return false;
}

bool BitBuffer::writeInt32(int data)
{
	if (writeDataType(7))
	{
		return write(32, &data);
	}

	return false;
}

bool BitBuffer::writeUInt32(unsigned int data)
{
	if (writeDataType(8))
	{
		return write(32, &data);
	}

	return false;
}

bool BitBuffer::writeBytes(int bytes, const BYTE* data)
{
	return write(bytes * 8, data);
}

bool BitBuffer::writeDataType(char data)
{
	if (!_useDataTypes)
	{
		return true;
	}

	return write(5, &data);
}

bool BitBuffer::writeType(char data)
{
	return write(5, &data);
}

bool BitBuffer::readDataType(char expected)
{
	if (!_useDataTypes)
	{
		return true;
	}

	char dataType;

	if (read(5, &dataType))
	{
		return (dataType == expected);
	}

	return false;
}

bool BitBuffer::readBytes(int bytes, BYTE* output)
{
	return read(bytes * 8, output);
}

bool BitBuffer::readUInt32(unsigned int* output)
{
	if (!readDataType(8))
	{
		return false;
	}

	return read(32, output);
}

bool BitBuffer::readBoolean(bool* output)
{
	if (!readDataType(1))
	{
		return false;
	}

	return read(1, output);
}

void BitBuffer::setUseDataTypes(bool useDataTypes)
{
	_useDataTypes = useDataTypes;
}

int BitBuffer::getLength()
{
	return (_curBit / 8) + ((_curBit % 8) ? 1 : 0);
}
}