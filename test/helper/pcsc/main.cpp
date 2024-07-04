/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

#include "pcscmock.h"

#include <QByteArray>
#include <QtGlobal>
#include <winscard.h>
#ifndef Q_OS_WIN
	#include <wintypes.h>
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

	const auto featuresTLV = QByteArray::fromHex("120442330012");
	Q_ASSERT(static_cast<qsizetype>(cbRecvLength) >= featuresTLV.size());
	memcpy(pbRecvBuffer, featuresTLV.data(), static_cast<size_t>(featuresTLV.size()));
	*lpBytesReturned = static_cast<DWORD>(featuresTLV.size());

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
