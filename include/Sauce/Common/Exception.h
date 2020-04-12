#pragma once

#include <Sauce/Config.h>
#include <Sauce/Common/Callstack.h>

BEGIN_SAUCE_NAMESPACE

class SAUCE_API Exception : public exception
{
public:

	/**
	 * \fn	Exception::Exception(RetCode code, const char * msg, ...);
	 *
	 * \brief	Constructor.
	 *
	 * \param	code	The code.
	 * \param	msg 	The message.
	 * \param	... 	Variable arguments providing additional information.
	 */

	Exception(RetCode code, const char * msg, ...);
	~Exception() {}

	RetCode errorCode() const
	{
		return m_errorCode;
	}

	string message() const
	{
		return m_message;
	}

	const char *what() const noexcept override { return m_message.c_str(); }

	string callstack() const
	{
#ifdef SAUCE_COMPILE_WINDOWS
		return m_callstack.toString();
#else
		return "Missing";
#endif
	}

private:
	/** \brief	The message. */
	string m_message;

	/** \brief	The error code. */
	RetCode m_errorCode;

#ifdef SAUCE_COMPILE_WINDOWS
	/** \brief Callstack of point of error */
	Callstack m_callstack;
#endif
};

END_SAUCE_NAMESPACE
