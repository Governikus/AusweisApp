/**
 * Copyright (c) 2023-2026 Governikus Service GmbH, Germany
 */

#include "pcscmock.h"

#include <QByteArray>
#include <QtGlobal>
#include <winscard.h>
#ifndef Q_OS_WIN
	#include <wintypes.h>
#endif

#if defined(Q_OS_MACOS)
	#define CM_IOCTL_GET_FEATURE_REQUEST (0x42000000 + 3400)
#elif defined(PCSCLITE_VERSION_NUMBER)
	#include <reader.h>
#else
//  PC/SC Part 10 v2.02.09 November 2012 - 2.2 GET_FEATURE_REQUEST
	#define CM_IOCTL_GET_FEATURE_REQUEST SCARD_CTL_CODE(3400)
#endif


struct MockSCardCommandData
{
	LONG mSCardGetStatusChange = 0;
}
mMockData;


void governikus::setResultGetCardStatus(LONG pReturnCode)
{
	mMockData.mSCardGetStatusChange = pReturnCode;
}


LONG governikus::getResultGetCardStatus()
{
	return mMockData.mSCardGetStatusChange;
}


LONG SCardEstablishContext(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext)
{
	Q_UNUSED(dwScope)
	Q_UNUSED(pvReserved1)
	Q_UNUSED(pvReserved2)
	* phContext = 4;

	return SCARD_S_SUCCESS;
}


LONG SCardConnect(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
	Q_ASSERT(hContext == 4);
	Q_UNUSED(szReader)
	Q_UNUSED(dwShareMode)
	Q_UNUSED(dwPreferredProtocols)
	* phCard = 8;
	Q_UNUSED(pdwActiveProtocol)

	return SCARD_S_SUCCESS;
}


LONG SCardControl(SCARDHANDLE hCard, DWORD dwControlCode, LPCVOID pbSendBuffer, DWORD cbSendLength, LPVOID pbRecvBuffer, DWORD cbRecvLength, LPDWORD lpBytesReturned)
{
	Q_ASSERT(hCard == 8);
	Q_UNUSED(dwControlCode)
	Q_UNUSED(pbSendBuffer)
	Q_UNUSED(cbSendLength)

	QByteArray output;
	switch (dwControlCode)
	{
		case CM_IOCTL_GET_FEATURE_REQUEST:
			output = QByteArray::fromHex("120442330012");
			break;

		case 0x42000dcc: // EXECUTE_PACE
			output = QByteArray::fromHex("c26306f0040090003100");
			break;

		default:
			return SCARD_E_INVALID_PARAMETER;
	}

	Q_ASSERT(static_cast<qsizetype>(cbRecvLength) >= output.size());
	memcpy(pbRecvBuffer, output.data(), static_cast<size_t>(output.size()));
	*lpBytesReturned = static_cast<DWORD>(output.size());

	return SCARD_S_SUCCESS;
}


LONG SCardDisconnect(SCARDHANDLE hCard, DWORD dwDisposition)
{
	Q_ASSERT(hCard == 8);
	Q_UNUSED(dwDisposition)

	return SCARD_S_SUCCESS;
}


LONG SCardGetStatusChange(SCARDCONTEXT hContext, DWORD dwTimeout, SCARD_READERSTATE* rgReaderStates, DWORD cReaders)
{
	Q_ASSERT(hContext == 4);
	Q_UNUSED(dwTimeout)
	Q_UNUSED(rgReaderStates)
	Q_UNUSED(cReaders)

	return governikus::getResultGetCardStatus();
}


LONG SCardCancel(SCARDCONTEXT hContext)
{
	Q_ASSERT(hContext == 4);

	return SCARD_S_SUCCESS;
}


LONG SCardReleaseContext(SCARDCONTEXT hContext)
{
	Q_ASSERT(hContext == 4);

	return SCARD_S_SUCCESS;
}
