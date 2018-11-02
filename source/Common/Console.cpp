//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Made by Marcus "Bitsauce" Loo Vergara
// 2011-2018 (C)

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

Console *Console::s_this = nullptr;

Console::Console() :
	m_engine(0), // Set by the engine
	m_output(0)
{
	// Check singleton
	if(s_this)
	{
		THROW("Console already initialized!");
	}
	s_this = this;
}

Console::~Console()
{
	delete m_output;
	s_this = 0;
}

void Console::call_log(const char *msg, va_list args)
{
#ifdef SAUCE_COMPILE_WINDOWS
	// Get string length
	int size = _vscprintf(msg, args);

	// Create out string
	string out;
	out.resize(size);
	
	// Parse varargs
#ifdef USE_CTR_SECURE
	vsprintf_s(&out[0], size + 1, msg, args);
#else
	vsprintf(&out[0], msg, args);
#endif

	// System log
#ifdef SAUCE_COMPILE_WINDOWS
	OutputDebugString(out.c_str());
	OutputDebugString("\n");
#endif
	
	// Append message to log file
	if(m_engine->isEnabled(SAUCE_EXPORT_LOG))
	{
		*m_output << out << endl;
		m_output->flush();
	}

	// Append to console buffer
	m_log.append(out);
	m_buffer.append(out);
#endif
}

#ifdef SAUCE_COMPILE_WINDOWS
#ifndef USE_CTR_SECURE
#define CALL_LOG(format, ...)						\
	int size = _scprintf(format, __VA_ARGS__) + 1;	\
	char *newMsg = new char[size];					\
	sprintf(newMsg, format, __VA_ARGS__);			\
	s_this->call_log(newMsg, args);					\
	delete newMsg;
#else
#define CALL_LOG(format, ...)						\
	int size = _scprintf(format, __VA_ARGS__) + 1;	\
	char *newMsg = new char[size];					\
	sprintf_s(newMsg, size, format, __VA_ARGS__);	\
	s_this->call_log(newMsg, args);					\
	delete newMsg;
#endif
#endif

void Console::Log(const char *function, const char *file, const int line, const char *msg, ...)
{
	// Get argument lists
	va_list args;
	va_start(args, msg);

	// Get timestamp
	time_t rawtime;
	time(&rawtime);
	tm *timeinfo = localtime(&rawtime);
	char timestamp[9];
	strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);

#ifdef SAUCE_COMPILE_WINDOWS
	CALL_LOG("[%s] [INFO] [%s:%i in function %s]\n%s", timestamp, file, line, function, msg);
#endif

	va_end(args);
}

string Console::getLog() const
{
	return m_log;
}

void Console::clear()
{
	m_log.clear();
}

string Console::readBuffer()
{
	string buffer = m_buffer;
	m_buffer.clear();
	return buffer;
}

bool Console::hasBuffer() const
{
	return m_buffer.size() > 0;
}

void Console::clearBuffer()
{
	m_buffer.clear();
}

END_SAUCE_NAMESPACE
