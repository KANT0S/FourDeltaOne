#pragma once

class bdByteBuffer
{
private:
	BYTE* _bytes;
	int _curByte;
	int _maxByte;

	bool _dataTypePacking;
	char _lastDataType;

public:
	bdByteBuffer() {}
	bdByteBuffer(char* bytes, int length);

	void init(char* bytes, int length);

	bool readByte(char* output);
	bool readBoolean(bool* output);
	bool readInt32(int* output);
	bool readUInt32(unsigned int* output);
	bool readInt64(__int64* output);
	bool readUInt64(unsigned __int64* output);
	bool readFloat(float* output);
	bool readString(char** output);
	bool readString(char* output, int length);
	bool readBlob(char** output, int* length);
	bool readDataType(char expected);

	bool writeByte(char data);
	bool writeInt32(int data);
	bool writeUInt32(unsigned int data);
	bool writeInt64(__int64 data);
	bool writeUInt64(unsigned __int64 data);
	bool writeDataType(char data);
	bool writeFloat(float data);
	bool writeString(const char* data);
	bool writeBlob(const char* data, int length);

	bool read(int bytes, void* output);
	bool write(int bytes, const void* data);

	bool setDataTypePacking(bool pack);
	int getLength();
};