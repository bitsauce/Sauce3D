// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>
#include <Sauce/Math.h>
#include <Sauce/Input.h>

BEGIN_SAUCE_NAMESPACE

/*********************************************************************
**	Game console													**
**********************************************************************/

class FileSystem;
class FileReader;
class FileWriter;
class AudioManager;
class Graphics;

SAUCE_FORWARD_DECLARE(Window);

#ifdef __LINUX__
extern int _vscprintf (const char * format, va_list pargs);
#endif

/**
 * \class	FileReader
 *
 * \brief	A file reader.
 */

class SAUCE_API Console
{
	friend class Game;
public:

	/**
	 * \fn	Console::Console();
	 *
	 * \brief	Default constructor.
	 */

	Console();
	~Console();

	/**
	 * \fn	void Console::log(const string &msg);
	 *
	 * \brief	Ouputs \p msg to the console
	 *
	 * \param	msg	The message.
	 */

	//void log(const string &msg);

	/**
	 * \fn	void Console::logf(const char *msg, ...);
	 *
	 * \brief	Logfs.
	 *
	 * \param	msg	The message.
	 * \param	...	Variable arguments providing additional information.
	 */

	void logf(const char *msg, ...);

	/**
	 * \fn	string Console::getLog() const;
	 *
	 * \brief	Gets the log.
	 *
	 * \return	The log.
	 */

	string getLog() const;

	/**
	 * \fn	void Console::clear();
	 *
	 * \brief	Clears this object to its blank/initial state.
	 */

	void clear();

	/**
	 * \fn	string Console::readBuffer();
	 *
	 * \brief	Reads the buffer.
	 *
	 * \return	The buffer.
	 */

	string readBuffer();
	bool hasBuffer() const;
	void clearBuffer();

	static void Log(const char *function, const char *file, const int line, const char *msg, ...);

private:

	 /**
	  * \fn	* fn void Console::call_log(const char *msg, va_list args);
	  *
	  * \brief	Call log.
	  *
	  * \param	msg 	The message.
	  * \param	args	The arguments.
	  */
	void call_log(const char *msg, va_list args);

	/** \brief	The log. */
	string m_log;

	/** \brief	The buffer. */
	string m_buffer;
	
	/** \brief	The engine. */
	Game *m_engine;
	
	/** \brief	The output. */
	ofstream *m_output;

	static Console *s_this;
///< .
};

/**
 * \brief	A macro for logging formatted messages
 *
 * \todo TODO: Add timestap to all console messages
 *
 * \param	str	The message to log
 * \param	...	Variable argument list to format \p message with
 */

#define LOG(str, ...)                   Console::Log(__FUNCTION__, __FILE__, __LINE__, str, __VA_ARGS__)
#define LOG_IF(cond, str, ...) if(cond) Console::Log(__FUNCTION__, __FILE__, __LINE__, str, __VA_ARGS__)

/**
 * \brief	A macro for throwing an exception with a formatted string
 *
 * \param	str	The message to throw
 * \param	...	Variable argument list to format \p message with
 */

#define THROW(str, ...)                   throw Exception(RetCode::RuntimeException, str, __VA_ARGS__)
#define THROW_IF(cond, str, ...) if(cond) throw Exception(RetCode::RuntimeException, str, __VA_ARGS__)

END_SAUCE_NAMESPACE

#include <Sauce/Common/ResourceManager.h>
#include <Sauce/Common/SceneObject.h>
#include <Sauce/Common/Event.h>
#include <Sauce/Common/Callstack.h>

BEGIN_SAUCE_NAMESPACE

/*********************************************************************
**	Event handler													**
**********************************************************************/

#ifdef __LINUX__
#include <stdint.h>

typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef int64_t LONGLONG;

typedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    LONG  HighPart;
  };
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;
#endif

#ifdef SAUCE_COMPILE_WINDOWS
class SAUCE_API Timer
{
public:

	 /**
	  * \fn	* fn Timer::Timer();
	  *
	  * \brief	Default constructor.
	  */

	Timer();
	
	// High-resolution timing
	void  start();

	/**
	 * \fn	void Timer::stop();
	 *
	 * \brief	Stops this object.
	 */

	void  stop();
	double getElapsedTime() const;

private:

	/** \brief	The frequency. */
	LARGE_INTEGER m_frequency;

	/** \brief	The start. */
	LARGE_INTEGER m_start;

	/** \brief	The end. */
	LARGE_INTEGER m_end;

	/** \brief	true to running. */
	bool m_running;
};

#else

class SAUCE_API Timer {
public:
    Timer() :
		m_start(clock_::now()),
		m_end(clock_::now())
	{
    }
    
	void start()
	{
        m_start = clock_::now();
    }

	void stop()
	{
		m_end = clock_::now();
	}

    double getElapsedTime() const
	{
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count();
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> m_start, m_end;
};

#endif // SAUCE_COMPILE_WINDOWS

class SAUCE_API SimpleTimer
{
public:
	SimpleTimer();

	void start();
	float stop();

	float getElapsedTime() const;

private:
	clock_t m_startTick;
	bool m_running;
};

/*********************************************************************
**	Thread manager													**
**********************************************************************/

class SAUCE_API ThreadManager
{
public:

	/**
	 * \fn	virtual void ThreadManager::setupThread() = 0;
	 *
	 * \brief	Sets up the thread.
	 */

	virtual void setupThread() = 0;

	/**
	 * \fn	virtual void ThreadManager::cleanupThread() = 0;
	 *
	 * \brief	Cleanup thread.
	 */

	virtual void cleanupThread() = 0;
///< .
};

#ifdef DEPRECATED
/*********************************************************************
**	File reader class												**
**********************************************************************/

class SAUCE_API FileReader
{
public:
	FileReader(const string &path);

	/**
	 * \fn	FileReader::~FileReader();
	 *
	 * \brief	Destructor.
	 */

	~FileReader();

	/**
	 * \fn	bool FileReader::isOpen();
	 *
	 * \brief	Query if this object is open.
	 *
	 * \return	true if open, false if not.
	 */

	bool isOpen();
	bool isEOF();
	void close();

	/**
	 * \fn	bool FileReader::readBytes(char *buffer, const int count);
	 *
	 * \brief	Reads the bytes.
	 *
	 * \param [in,out]	buffer	If non-null, the buffer.
	 * \param	count		  	Number of.
	 *
	 * \return	true if it succeeds, false if it fails.
	 */

	bool readBytes(char *buffer, const int count);
	string readLine();
	string readAll();

	FileReader &operator>>(int &i) { stream >> i; return *this; }

	/**
	 * \fn	FileReader FileReader::&operator>>(char &c)
	 *
	 * \brief	Bitwise right shift operator.
	 *
	 * \param [in,out]	c	The char to process.
	 *
	 * \return	The shifted result.
	 */

	FileReader &operator>>(char &c) { stream >> c; return *this; }
	FileReader &operator>>(uchar &c)
	{
		stream >> c; return *this;
	}
	FileReader &operator>>(float &f)
	{
		stream >> f; return *this;
	}

private:
	/** \brief	The stream. */
	/** \brief	The stream. */
	ifstream stream;
};

/*********************************************************************
**	File writer class												**
**********************************************************************/

class SAUCE_API FileWriter
{
public:
	FileWriter(const string &path);

	/**
	 * \fn	FileWriter::~FileWriter();
	 *
	 * \brief	Destructor.
	 */

	~FileWriter();

	bool isOpen();
	void close();

	void clear();
	void append(const char* data, const int length);

	/**
	 * \fn	void FileWriter::append(const string &str)
	 *
	 * \brief	Appends a str.
	 *
	 * \param	str	The string to append.
	 */

	void append(const string &str) { append(str.data(), str.size()); }
	void flush();

    /**
     * \fn	FileWriter FileWriter::&operator<<(basic_ostream<char, std::char_traits<char> >& (*fp)(basic_ostream<char, std::char_traits<char> >&))
     *
     * \brief	define an operator<< to take in std::endl.
     *
     * \param [in,out]	fp	If non-null, the fp.
     *
     * \return	The shifted result.
     */

    FileWriter &operator<<(basic_ostream<char, std::char_traits<char> >& (*fp)(basic_ostream<char, std::char_traits<char> >&)) { stream << fp; return *this; }

	/**
	 * \fn	bool FileWriter::operator!() const
	 *
	 * \brief	Logical negation operator.
	 *
	 * \return	The logical inverse of this value.
	 */

	bool operator!() const { return !stream; }
	FileWriter &operator<<(int i) { stream << i; return *this; }
	FileWriter &operator<<(uint i) { stream << i; return *this; }

	/**
	 * \fn	FileWriter FileWriter::&operator<<(char c)
	 *
	 * \brief	Bitwise left shift operator.
	 *
	 * \param	c	The character.
	 *
	 * \return	The shifted result.
	 */

	FileWriter &operator<<(char c) { stream << c; return *this; }

	FileWriter &operator<<(float f)
	{
		stream << f; return *this;
	}

private:
	/** \brief	The stream. */
	/** \brief	The stream. */
	ofstream stream;
};

/*********************************************************************
**	File writer class												**
**********************************************************************/

class SAUCE_API FileSystemIterator
{
public:
	FileSystemIterator(string path, const string &mask, const int flags);
	//~FileSystemIterator();

	/**
	 * \enum	Flag
	 *
	 * \brief	Values that represent flags.
	 */

	enum Flag
	{
		///< An enum constant representing the directories option
		DIRECTORIES = 1 << 1,
		///< An enum constant representing the files option
		FILES = 1 << 2
	};

	/**
	 * \fn	bool FileSystemIterator::hasNext() const;
	 *
	 * \brief	Query if this object has next.
	 *
	 * \return	true if next, false if not.
	 */

	bool hasNext() const;
	string &next();

	operator bool() const { return hasNext(); }

private:
	/** \brief	The files. */
	/** \brief	The files. */
	/** \brief	The files. */
	vector<string> m_files;
	/** \brief	The itr. */
	vector<string>::iterator m_itr;
};


/*********************************************************************
**	File system class												**
**********************************************************************/

/**
 * \class	SAUCE_API
 *
 * \brief	TODO: Needs re-doing with SDL in mind.
 */

class SAUCE_API FileSystem
{
	/**
	 * \class	Game
	 *
	 * \brief	A game.
	 */

	friend class Game;
public:

	/**
	 * \fn	bool FileSystem::readFile(string filePath, string &conent) const;
	 *
	 * \brief	File buffers.
	 *
	 * \param	filePath	  	Full pathname of the file.
	 * \param [in,out]	conent	The conent.
	 *
	 * \return	true if it succeeds, false if it fails.
	 */

	bool readFile(string filePath, string &conent) const;
	bool writeFile(string filePath, const string content) const;

	/**
	 * \fn	bool FileSystem::fileExists(string &filePath) const;
	 *
	 * \brief	OS specifics.
	 *
	 * \param [in,out]	filePath	Full pathname of the file.
	 *
	 * \return	true if it succeeds, false if it fails.
	 */

	bool fileExists(string &filePath) const;

	// Static functions
	static bool ReadFile(string path, string &content);
	static bool WriteFile(string path, const string &content);

	/**
	 * \fn	static bool FileSystem::MakeDir(string path);
	 *
	 * \brief	Makes a dir.
	 *
	 * \param	path	Full pathname of the file.
	 *
	 * \return	true if it succeeds, false if it fails.
	 */

	static bool MakeDir(string path);

protected:
	bool makeDir(const string &path);

private:
	/** \brief	this. */
	static FileSystem *s_this;
};
#endif

/*********************************************************************
**	Scene class													**
**********************************************************************/

class SAUCE_API Scene
{
	friend class Game;
public:
	sauce::SceneObject *getRoot() const
	{
		return m_root;
	}

private:
	Scene(SceneObject *root) :
		m_root(root)
	{
	}

	SceneObject *m_root;
};

struct SAUCE_API GameDesc
{
	string          name             = "DefaultGame";
	string          workingDirectory = ".";
	string          organization     = "Sauce3D";
	uint32          flags            = 0;
	GraphicsBackend graphicsBackend  = GraphicsBackend::OpenGL4;
	double          deltaTime        = 1.0 / 30.0;
};

class ResourceManager;

class SAUCE_API Game : public SceneObject
{
public:
	Game();
	~Game();

	// Run game
	int run(const GameDesc &desc);
	
	// End game
	void end();

	// Set pause state
	void setPaused(const bool paused);

	// Check game config
	uint getFlags() const;
	bool isEnabled(const EngineFlag flag);

	// Get binary path
	string getBinaryPath() const { return m_binaryPath; }

	// Get save dir
	string getPrefPath() const { return m_prefPath; }

	/**
	 * \fn	Window Game::*getWindow(const Sint32 id = -1) const;
	 *
	 * \brief	Get a game window.
	 *
	 * \param	id	The identifier. If less than 0, the functon will return the first window created.
	 *
	 * \return	null a window with \p id does not exist, else it returns the window.
	 */
	WindowRef getWindow(const int32 windowIndex = -1) const;

	/**
	 * \fn	float Game::getFPS() const
	 *
	 * \brief	Gets the FPS.
	 *
	 * \return	The FPS.
	 */

	float getFPS() const
	{
		return m_framesPerSecond;
	}

	InputManager *getInputManager()
	{
		return m_inputManager;
	}

	ResourceManager *getResourceManager()
	{
		return m_resourceManager;
	}

	Scene *getScene()
	{
		return m_scene;
	}

	static Game *Get()
	{
		return s_this;
	}

private:

	GameDesc m_desc;
	
	/** \brief	Initialized? */
	bool m_initialized;

	/** \brief	Paused? */
	bool m_paused;

	/** \brief	Running? */
	bool m_running;

	/** \brief	Fps. */
	double m_framesPerSecond;
	
	/** \brief	Game windows. */
	list<WindowRef> m_windows;

	/** \brief	The file system. */
	FileSystem		*m_fileSystem;
	
	//AudioManager	*m_audio;
	
	ResourceManager *m_resourceManager;
	
	InputManager *m_inputManager;

	Scene *m_scene;
	
	/** \brief	The timer. */
	Timer			*m_timer;
	
	/** \brief	The console. */
	Console			*m_console;

	/**
	 * \property	string m_binaryPath, m_prefPath
	 *
	 * \brief	Engine working directory.
	 *
	 * \return	The full pathname of the preference file.
	 */

	string m_binaryPath, m_prefPath;

	// Static game
	static Game *s_this;
};

END_SAUCE_NAMESPACE
