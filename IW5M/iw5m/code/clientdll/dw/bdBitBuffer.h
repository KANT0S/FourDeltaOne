#pragma once

class bdBitBuffer
{
private:
	BYTE* _bytes;
	int _curBit;
	int _maxBit;

	bool _useDataTypes;

public:
	bdBitBuffer() {}
	bdBitBuffer(char* bytes, int length);

	void init(char* bytes, int length);

	bool readBytes(int bytes, BYTE* output);
	bool readBoolean(bool* output);
	bool readUInt32(unsigned int* output);
	bool readDataType(char expected);

	bool writeBytes(int bytes, const BYTE* data);
	bool writeBoolean(bool data);
	bool writeInt32(int data);
	bool writeUInt32(unsigned int data);
	bool writeDataType(char data);

	bool read(int bits, void* output);
	bool write(int bits, const void* data);

	void setUseDataTypes(bool useDataTypes);

	int getLength();
};