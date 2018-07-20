#include "StdInc.h"
#include "bdByteBuffer.h"

bdByteBuffer::bdByteBuffer(char* bytes, int length)
{
	init(bytes, length);
}

void bdByteBuffer::init(char* bytes, int length)
{
	_bytes = (BYTE*)bytes;
	_curByte = 0;
	_maxByte = length;
	_dataTypePacking = false;
	_lastDataType = 0;
}

bool bdByteBuffer::write(int bytes, const void* data)
{
	if ((bytes + _curByte) > _maxByte)
	{
		return false;
	}

	memcpy(_bytes + _curByte, data, bytes);
	_curByte += bytes;

	return true;
}

bool bdByteBuffer::read(int bytes, void* output)
{
	if ((bytes + _curByte) > _maxByte)
	{
		return false;
	}

	memcpy(output, _bytes + _curByte, bytes);
	_curByte += bytes;

	return true;
}

bool bdByteBuffer::readDataType(char expected)
{
	if (_dataTypePacking && expected == _lastDataType)
	{
		return true;
	}

	char type;
	read(1, &type);

	if (type != expected)
	{
		Trace("bdByteBuffer", "invalid data type (got %d, expected %d)", type, expected);
		return false;
	}

	_lastDataType = type;
	return true;
}

bool bdByteBuffer::readBlob(char** output, int* length)
{
	readDataType(0x13);

	unsigned int size;
	readUInt32(&size);

	*output = (char*)&_bytes[_curByte];
	*length = size;

	_curByte += size;

	return true;
}

bool bdByteBuffer::readBoolean(bool* output)
{
	readDataType(1);

	return read(1, output);
}

bool bdByteBuffer::readFloat(float* output)
{
	readDataType(13);

	return read(4, output);
}

bool bdByteBuffer::readInt32(int* output)
{
	readDataType(7);

	return read(4, output);
}

bool bdByteBuffer::readUInt32(unsigned int* output)
{
	readDataType(8);

	return read(4, output);
}

bool bdByteBuffer::readInt64(__int64* output)
{
	readDataType(9);

	return read(8, output);
}

bool bdByteBuffer::readUInt64(unsigned __int64* output)
{
	readDataType(10);

	return read(8, output);
}

bool bdByteBuffer::readByte(char* output)
{
	readDataType(3);

	return read(1, output);
}

bool bdByteBuffer::readString(char** output)
{
	readDataType(16);

	*output = (char*)&_bytes[_curByte];

	_curByte += strlen(*output) + 1;

	return true;
}

bool bdByteBuffer::readString(char* output, int length)
{
	readDataType(16);

	char* string = (char*)&_bytes[_curByte];
	strcpy_s(output, length, string);

	_curByte += strlen(string) + 1;

	return true;
}

bool bdByteBuffer::writeBlob(const char* data, int length)
{
	writeDataType(0x13);
	writeUInt32(length);

	return write(length, data);
}

bool bdByteBuffer::writeByte(char data)
{
	writeDataType(3);

	return write(1, &data);
}

bool bdByteBuffer::writeInt32(int data)
{
	writeDataType(7);

	return write(4, &data);
}

bool bdByteBuffer::writeUInt32(unsigned int data)
{
	writeDataType(8);

	return write(4, &data);
}

bool bdByteBuffer::writeInt64(__int64 data)
{
	writeDataType(9);

	return write(8, &data);
}

bool bdByteBuffer::writeUInt64(unsigned __int64 data)
{
	writeDataType(10);

	return write(8, &data);
}

bool bdByteBuffer::writeString(const char* data)
{
	writeDataType(16);

	return write(strlen(data) + 1, data);
}

bool bdByteBuffer::writeFloat(float data)
{
	writeDataType(13);

	return write(4, &data);
}

bool bdByteBuffer::writeDataType(char data)
{
	return write(1, &data);
}

int bdByteBuffer::getLength()
{
	return (_curByte);
}