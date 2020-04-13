//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

#ifdef __LINUX__
int _vscprintf(const char* format, va_list pargs) {
	int retval;
	va_list argcopy;
	va_copy(argcopy, pargs);
	retval = vsnprintf(NULL, 0, format, argcopy);
	va_end(argcopy);
	return retval;
}
#endif

//--------------------------------------------------------------------
// Exception class
//--------------------------------------------------------------------

Exception::Exception(RetCode code, const char* msg, ...) :
	m_errorCode(code)
#ifdef SAUCE_COMPILE_WINDOWS
	, m_callstack()
#endif
{
	va_list args;
	va_start(args, msg);

	// Get string length
	const size_t size = _vscprintf(msg, args);

	// Create out string
	m_message.resize(size);

	// Parse varargs
#ifdef USE_CTR_SECURE
	vsprintf_s(&m_message[0], size + 1, msg, args);
#else
	vsprintf(&m_message[0], msg, args);
#endif

	va_end(args);

#ifdef SAUCE_COMPILE_WINDOWS
	// Break if debugger is present
	if (IsDebuggerPresent())
	{
		LOG("%s", m_message.c_str());
		DebugBreak();
	}
#endif
}

END_SAUCE_NAMESPACE
