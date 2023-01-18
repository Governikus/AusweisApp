/*!
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpRequest.h"

#ifdef Q_OS_WIN
	#include <windows.h>

	#include <iphlpapi.h>
	#include <tlhelp32.h>
#else
	#include <QFileInfoList>
#endif

namespace governikus
{
class PortWrapper
{
	private:
#ifdef Q_OS_WIN
		quint16 mPort;

#else
		QFileInfoList mPortFiles;
#endif

#ifdef Q_OS_WIN
		static QString getUserOfProcessID(DWORD pPid);
		static QString getExecutableOfProcessID(DWORD pPid);
		static quint16 getPortOfRunningProcess(const QVector<MIB_TCPROW_OWNER_PID>& pConnections, const QString& pUser, int pSelfPort, const in_addr& pRemoteAddr);
		static QString getUserOfConnection(const QVector<MIB_TCPROW_OWNER_PID>& pConnections, int pLocalPort, int pRemotePort, const in_addr& pProxyAddr);
		static QVector<MIB_TCPROW_OWNER_PID> getConnections();
		static quint16 getProcessPort(int pLocalPort, int pRemotePort);
#else
		static quint16 readPortFile(const QString& pFile);
#endif

	public:
		explicit PortWrapper(const QSharedPointer<HttpRequest>& pRequest);

		[[nodiscard]] bool isEmpty() const;
		quint16 fetchPort();
		void invalidate();
};

} // namespace governikus
