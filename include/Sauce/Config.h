#ifndef SAUCE_CONFIG_H
#define SAUCE_CONFIG_H

#define NOMINMAX

#include <SDL.h>

/*********************************************************************
**	Compiler preprocessor											**
**********************************************************************/
#if defined(__GNUC__) || defined(__GNUG__)
	#define SAUCE_COMPILE_LINUX
#elif defined(_MSC_VER)
	#define SAUCE_COMPILE_WINDOWS
#endif

#if defined(SAUCE_COMPILE_WINDOWS) && _MSC_VER >= 1500
	#define USE_CTR_SECURE
#endif
#if defined(_DEBUG) && !defined(SAUCE_DEBUG) 
	#define SAUCE_DEBUG
#endif

/*********************************************************************
**	Library export preprocessor										**
**********************************************************************/
#if defined(SAUCE_COMPILE_WINDOWS) && defined(SAUCE_EXPORT)
	#define SAUCE_API __declspec(dllexport)
#elif defined(SAUCE_COMPILE_WINDOWS) && defined(SAUCE_IMPORT)
	#define SAUCE_API __declspec(dllimport)
#else
	#define SAUCE_API 
#endif

/*********************************************************************
**	Include	STL														**
**********************************************************************/
#ifdef SAUCE_COMPILE_WINDOWS
	#include <string>
	#include <vector>
	#include <list>
	#include <cmath>
	#include <assert.h>
	#include <algorithm>
	#include <functional>
	#include <map>
	#include <unordered_map>
	#include <set>
	#include <stack>
	#include <exception>
	#include <sstream>
	#include <thread>
	#include <mutex>
	#include <assert.h>
	#include <fstream>
	#include <sstream>
	#include <memory>
	#include <queue>
	#include <chrono>
	#include "..\3rdparty\gl3w\include\GL\gl3w.h"
	#include "..\3rdparty\gl3w\include\GL\wglext.h"
#elif __LINUX__
	#include <string>
	#include <vector>
	#include <list>
	#include <cmath>
	#include <assert.h>
	#include <algorithm>
	#include <functional>
	#include <map>
	#include <unordered_map>
	#include <set>
	#include <stack>
	#include <exception>
	#include <sstream>
	#include <thread>
	#include <mutex>
	#include <assert.h>
	#include <fstream>
	#include <sstream>
	#include <memory>
	#include <queue>
	#include <chrono>
	#include <GL/glew.h>
	#include <GL/glut.h>
	

	#undef major
	#undef minor
#endif

/*********************************************************************
**	Enable STL namespace											**
**********************************************************************/
using namespace std;

/*********************************************************************
**	Set typedefs													**
**********************************************************************/
// TODO: Should get rid of these old typedefs
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;

#ifdef SAUCE_USE_FLOAT
typedef float  SFloat;
#else
typedef double SFloat;
#endif

#define BEGIN_SAUCE_NAMESPACE namespace sauce {
#define END_SAUCE_NAMESPACE }

BEGIN_SAUCE_NAMESPACE

/*********************************************************************
**	Engine return codes												**
**********************************************************************/
enum RetCode
{
	SAUCE_OK					= 0,
	SAUCE_RUNTIME_EXCEPTION		= -1,
	SAUCE_INVALID_CONFIG		= -2,
	SAUCE_UNKNOWN_EXCEPTION		= -3
};

/*********************************************************************
**	Engine run flags												**
**********************************************************************/
enum class EngineFlag : uint32
{
	SAUCE_EXPORT_LOG				= 1 << 0, ///< Export the output log to a log file.
	SAUCE_RUN_IN_BACKGROUND			= 1 << 1, ///< This will allow the program to run while not focused.
	SAUCE_BLOCK_BACKGROUND_INPUT	= 1 << 2, ///< If SAUCE_RUN_IN_BACKGROUND is set, this will block input while program is out of focus. 
	SAUCE_VERBOSE					= 1 << 4, ///< This will make the engine produce more verbose messages from engine calls.
	SAUCE_WINDOW_RESIZABLE			= 1 << 5
};

/*********************************************************************
**	Message types													**
**********************************************************************/
enum MessageType
{
	SAUCE_INFO_MSG,
	SAUCE_WARN_MSG,
	SAUCE_ERR_MSG
};

/*********************************************************************
**	List of supported graphics backends								**
**********************************************************************/
enum class GraphicsBackend : uint32
{
	SAUCE_OPENGL_3,
	SAUCE_OPENGL_4,
	SAUCE_DIRECTX,
	SAUCE_VULKAN
};

/*********************************************************************
**	Global util functions											**
**********************************************************************/
namespace util
{
	// Split string
	SAUCE_API vector<string> splitString(const string& src, const string& delim);

	// Replace all
	SAUCE_API void replaceAll(string& str, string& from, string& to);
	
	// String case functions
	SAUCE_API string toLower(string &str, const int begin = 0, const int end = 0);
	SAUCE_API string toUpper(string &str, const int begin = 0, const int end = 0);

	// String convertering
	SAUCE_API int   strToInt(const string &str);
	SAUCE_API float strToFloat(const string &str);
	SAUCE_API bool  strToBool(const string &str);
	SAUCE_API uchar strToAscii(const string&);

	SAUCE_API string intToStr(const int);
	SAUCE_API string floatToStr(const float);
	SAUCE_API string boolToStr(const bool);
	SAUCE_API string asciiToStr(const uchar);
	
	// File paths
	SAUCE_API bool fileExists(string filePath);
	SAUCE_API string getAbsoluteFilePath(const string &assetPath);
	SAUCE_API void toAbsoluteFilePath(string &assetPath);
	SAUCE_API void toDirectoryPath(string &path);
	SAUCE_API string getWorkingDirectory();

	enum UnicodeByteOrder
	{
		DECODE_LITTLE_ENDIAN,
		DECODE_BIG_ENDIAN,
	};

	// This function will attempt to decode a UTF-8 encoded character in the buffer.
	// If the encoding is invalid, the function returns -1.
	int decodeUTF8(const char *encodedBuffer, unsigned int *outCharLength);

	// This function will encode the value into the buffer.
	// If the value is invalid, the function returns -1, else the encoded length.
	int encodeUTF8(unsigned int value, char *outEncodedBuffer, unsigned int *outCharLength);

	// This function will attempt to decode a UTF-16 encoded character in the buffer.
	// If the encoding is invalid, the function returns -1.
	int decodeUTF16(const char *encodedBuffer, unsigned int *outCharLength, UnicodeByteOrder byteOrder = DECODE_LITTLE_ENDIAN);

	// This function will encode the value into the buffer.
	// If the value is invalid, the function returns -1, else the encoded length.
	int encodeUTF16(unsigned int value, char *outEncodedBuffer, unsigned int *outCharLength, UnicodeByteOrder byteOrder = DECODE_LITTLE_ENDIAN);
}

END_SAUCE_NAMESPACE

/*********************************************************************
**	Macros															**
**********************************************************************/

#define TUPLE_CMP(a, b) \
	if(a < b) return true; \
	if(a > b) return false;

#define TUPLE_CMP2(a, b) \
	if(a < b) return false; \
	if(a > b) return true;

#endif // SAUCE_CONFIG_H
