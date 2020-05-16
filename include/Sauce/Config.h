// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

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
	#define NOMINMAX
	#define _HAS_STD_BYTE 0
	#define _HAS_STD_BOOLEAN 0
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
	#include <filesystem>
	#include <regex>
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
**	Ref types														**
**********************************************************************/
#define SAUCE_REF_TYPE(Class)                                     \
	using RefType = shared_ptr<Class>;                            \
	using DescType = Class ## Desc;                               \
	typedef Class::RefType Class ## Ref;                          \
	typedef Class::DescType Class ## Desc;                        \
	bool initialize(SauceObjectDesc* objectDesc) override         \
	{                                                             \
		DescType* descType = dynamic_cast<DescType*>(objectDesc); \
		assert(descType);                                         \
		return descType ? initialize(*descType) : false;          \
	}

#define SAUCE_REF_TYPE_TYPEDEFS(Class)      \
	typedef Class::RefType Class ## Ref;    \
	typedef Class::DescType Class ## Desc

#define SAUCE_FORWARD_DECLARE(Class)        \
    class Class;                            \
    class Class ## Desc;                    \
	typedef shared_ptr<Class> Class ## Ref

/*********************************************************************
**	Enum class utility												**
**********************************************************************/
#define ENUM_CLASS_ADD_BITWISE_OPERATORS(Enumclass)                            \
	template<typename Other>                                                   \
	inline auto operator|(const Other& lhs, const Enumclass& rhs)              \
	{                                                                          \
		using T = std::underlying_type_t<Enumclass>;                           \
		return static_cast<T>(static_cast<T>(lhs) | static_cast<T>(rhs));      \
	}                                                                          \
                                                                               \
	template<typename Other>                                                   \
	inline bool operator&(const Other& lhs, const Enumclass& rhs)              \
	{                                                                          \
		using T = std::underlying_type_t<Enumclass>;                           \
		return static_cast<T>(static_cast<T>(lhs) & static_cast<T>(rhs)) != 0; \
	}

/*********************************************************************
**	Engine return codes												**
**********************************************************************/
enum class RetCode : int32
{
	Ok               =  0,
	RuntimeException = -1,
	InvalidConfig    = -2,
	UnknownException = -3
};

/*********************************************************************
**	Engine run flags												**
**********************************************************************/
enum class EngineFlag : uint32
{
	ExportLog                  = 1 << 0, ///< Export the output log to a log file.
	RunInBackground            = 1 << 1, ///< This will allow the program to run while not focused.
	CaptureInputWhenOutOfFocus = 1 << 2, ///< If SAUCE_RUN_IN_BACKGROUND is set, this will block input while program is out of focus. 
	Verbose                    = 1 << 4, ///< This will make the engine produce more verbose messages from engine calls.
	ResizableWindow            = 1 << 5
};
ENUM_CLASS_ADD_BITWISE_OPERATORS(EngineFlag);

/*********************************************************************
**	Message types													**
**********************************************************************/
enum MessageType
{
	Info,
	Warning,
	Error
};

/*********************************************************************
**	List of supported graphics backends								**
**********************************************************************/
enum class GraphicsBackend : uint32
{
	OpenGL4,
	DirectX12,
	Vulkan_VERSION
};

/*********************************************************************
**	Data type enum													**
**********************************************************************/
enum class Datatype : uint32
{
	Float,
	Uint32,
	Int32,
	Uint16,
	Int16,
	Uint8,
	Int8,

	Matrix4,
	Struct
};

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
