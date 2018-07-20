// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Bitstream class.
//
// Initial author: NTAuthority
// Started: 2013-02-14
// ==========================================================

#pragma once

namespace ci
{
	class BitBuffer
	{
	private:
		BYTE* _bytes;
		int _curBit;
		int _maxBit;

		bool _useDataTypes;
		bool _bytesManaged;

	public:
		BitBuffer();
		BitBuffer(char* bytes, int length);

		~BitBuffer();

		void init(char* bytes, int length);

		bool readBytes(int bytes, BYTE* output);
		bool readBoolean(bool* output);
		bool readUInt32(unsigned int* output);
		bool readDataType(char expected);

		bool writeBytes(int bytes, const BYTE* data);
		bool writeBoolean(bool data);
		bool writeInt32(int data);
		bool writeUInt32(unsigned int data);
		bool writeType(char data);
		bool writeDataType(char data);

		bool read(int bits, void* output);
		bool write(int bits, const void* data);

		void setUseDataTypes(bool useDataTypes);

		int getLength();
		BYTE* getBytes()
		{
			return _bytes;
		}
	};
}