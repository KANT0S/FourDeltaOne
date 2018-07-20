#pragma once

class bdByteBuffer
{
private:
	BYTE* _bytes;
	int _curByte;
	int _maxByte;

	bool _bytesManaged;

	bool _dataTypePacking;
	bool _useDataTypes;
	char _lastDataType;

public:
	bdByteBuffer();

	bdByteBuffer(char* bytes, int length);
	bdByteBuffer(std::string& string);

	~bdByteBuffer();

	void init(char* bytes, int length);
	void unmanageBytes()
	{
		_bytesManaged = false;
	}

	bool readByte(char* output);
	bool readBoolean(bool* output);
	bool readUInt16(unsigned short* output);
	bool readInt32(int* output);
	bool readUInt32(unsigned int* output);
	bool readInt64(__int64* output);
	bool readUInt64(unsigned __int64* output);
	bool readFloat(float* output);
	bool readString(char** output);
	bool readString(char* output, int length);
	bool readBlob(char** output, int* length);
	bool readBlob(std::string& output);

	bool readArray(char type, uint32_t* count, uint32_t* elSize, void** bufferPoint);

	bool readDataType(char expected);

	bool writeByte(char data);
	bool writeUInt16(unsigned short data);
	bool writeInt32(int data);
	bool writeUInt32(unsigned int data);
	bool writeInt64(__int64 data);
	bool writeUInt64(unsigned __int64 data);
	bool writeDataType(char data);
	bool writeFloat(float data);
	bool writeString(const char* data);
	bool writeBlob(const char* data, int length);
	bool writeBlob(std::string& data);

	bool writeArray(char type, uint32_t count, uint32_t elSize, const void* source);

	bool read(int bytes, void* output);
	bool write(int bytes, const void* data);

	bool setDataTypePacking(bool pack);
	int getLength();

	BYTE* getBytes() { return _bytes; }
};