/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QList>

#ifdef Q_OS_WIN
	#include <windows.h>

	#include <iphlpapi.h>
	#include <tlhelp32.h>
#endif


namespace governikus
{
class PortWrapper
{
	private:
		QList<quint16> mPorts;

#ifdef Q_OS_WIN
		static QString getUserOfProcessID(DWORD pPid);
		static QString getExecutableOfProcessID(DWORD pPid);
		static quint16 getPortOfRunningProcess(const MIB_TCPROW_OWNER_PID& pConnection, const QString& pUser, quint16 pLocalPort);
		static QString getUserOfConnection(const QList<MIB_TCPROW_OWNER_PID>& pConnections, quint16 pLocalPort, quint16 pPeerPort);
		static QList<MIB_TCPROW_OWNER_PID> getConnections();
		static quint16 getProcessPort(quint16 pLocalPort, quint16 pPeerPort);
#else
		static quint16 readPortFile(const QString& pFile);
#endif

	public:
		explicit PortWrapper(quint16 pLocalPort, quint16 pPeerPort = 0);

		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] quint16 fetchPort() const;
		void invalidate();
};

} // namespace governikus
