/*!
 * PcscUtils.h
 *
 * \brief toString method for PCSC_RETURNCODE and platform dependent
 * typedefs for PCSC types.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QString>
#include <QtGlobal>

#ifdef Q_OS_WIN
#include <winscard.h>
typedef LONG PCSC_RETURNCODE;
typedef SCARDHANDLE PCSC_CARDHANDLE;
typedef DWORD PCSC_UINT;
typedef DWORD PCSC_INT;
typedef DWORD* PCSC_INT_PTR;
#ifdef UNICODE
typedef WCHAR PCSC_CHAR;
#else
typedef CHAR PCSC_CHAR;
#endif
typedef LPTSTR PCSC_CHAR_PTR;
typedef BYTE PCSC_UCHAR;
typedef LPBYTE PCSC_UCHAR_PTR;
typedef LPCBYTE PCSC_CUCHAR_PTR;
#elif defined Q_OS_OSX
#include <winscard.h>
// The return type is actually int32_t (in the system headers), but the return
// code macros are defined without a cast operator, which makes them
// unsigned int due to a missing cast.
typedef uint32_t PCSC_RETURNCODE;
typedef SCARDHANDLE PCSC_CARDHANDLE;
typedef uint32_t PCSC_INT;
typedef PCSC_INT* PCSC_INT_PTR;
typedef char PCSC_CHAR;
typedef char* PCSC_CHAR_PTR;
typedef uchar PCSC_UCHAR;
typedef uchar* PCSC_UCHAR_PTR;
typedef const uchar* PCSC_CUCHAR_PTR;
#elif defined Q_OS_UNIX
#include <winscard.h>
typedef LONG PCSC_RETURNCODE;
typedef SCARDHANDLE PCSC_CARDHANDLE;
typedef DWORD PCSC_INT;
typedef PCSC_INT* PCSC_INT_PTR;
typedef char PCSC_CHAR;
typedef char* PCSC_CHAR_PTR;
typedef uchar PCSC_UCHAR;
typedef uchar* PCSC_UCHAR_PTR;
typedef const uchar* PCSC_CUCHAR_PTR;
#endif

namespace governikus
{

class PcscUtils
{
	private:
		PcscUtils() = delete;
		~PcscUtils() = delete;

	public:
		static QString toString(PCSC_RETURNCODE pCode);
};


} /* namespace governikus */
