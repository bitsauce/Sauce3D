// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#include <Sauce/Config.h>

#pragma once

BEGIN_SAUCE_NAMESPACE

namespace util
{
	// Split string
	SAUCE_API vector<string> splitString(const string& src, const string& delim);

	// Replace all
	SAUCE_API void replaceAll(string& str, string& from, string& to);

	// String case functions
	SAUCE_API string toLower(string& str, const int begin = 0, const int end = 0);
	SAUCE_API string toUpper(string& str, const int begin = 0, const int end = 0);

	// String convertering
	SAUCE_API int   strToInt(const string& str);
	SAUCE_API float strToFloat(const string& str);
	SAUCE_API bool  strToBool(const string& str);
	SAUCE_API uchar strToAscii(const string&);

	SAUCE_API string intToStr(const int);
	SAUCE_API string floatToStr(const float);
	SAUCE_API string boolToStr(const bool);
	SAUCE_API string asciiToStr(const uchar);

	enum class UnicodeByteOrder : uint32
	{
		DECODE_LITTLE_ENDIAN,
		DECODE_BIG_ENDIAN,
	};

	// This function will attempt to decode a UTF-8 encoded character in the buffer.
	// If the encoding is invalid, the function returns -1.
	int decodeUTF8(const char* encodedBuffer, unsigned int* outCharLength);

	// This function will encode the value into the buffer.
	// If the value is invalid, the function returns -1, else the encoded length.
	int encodeUTF8(unsigned int value, char* outEncodedBuffer, unsigned int* outCharLength);

	// This function will attempt to decode a UTF-16 encoded character in the buffer.
	// If the encoding is invalid, the function returns -1.
	int decodeUTF16(const char* encodedBuffer, unsigned int* outCharLength, UnicodeByteOrder byteOrder = UnicodeByteOrder::DECODE_LITTLE_ENDIAN);

	// This function will encode the value into the buffer.
	// If the value is invalid, the function returns -1, else the encoded length.
	int encodeUTF16(unsigned int value, char* outEncodedBuffer, unsigned int* outCharLength, UnicodeByteOrder byteOrder = UnicodeByteOrder::DECODE_LITTLE_ENDIAN);

	SAUCE_API std::string ByteArrayMD5(const std::string& str);

	SAUCE_API uint32 GetDatatypeSize(const Datatype datatype);
}

END_SAUCE_NAMESPACE
