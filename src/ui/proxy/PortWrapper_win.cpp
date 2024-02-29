/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#include "PortWrapper.h"

#include <QCoreApplication>
#include <QFile>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(rproxy)


PortWrapper::PortWrapper(const QSharedPointer<HttpRequest>& pRequest)
	: mPort(getProcessPort(pRequest->getLocalPort(), pRequest->getPeerPort()))
{
}


bool PortWrapper::isEmpty() const
{
	return mPort == 0;
}


void PortWrapper::invalidate()
{
	mPort = 0;
}


quint16 PortWrapper::fetchPort()
{
	return mPort;
}


QString PortWrapper::getUserOfProcessID(DWORD pPid)
{
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, false, pPid);
	if (!processHandle)
	{
		qCWarning(rproxy) << "Cannot call PROCESS_QUERY_INFORMATION:" << GetLastError();
		return QString();
	}
	const auto processHandleGuard = qScopeGuard([&processHandle]{
			CloseHandle(processHandle);
		});

	HANDLE processToken = nullptr;
	const auto processTokenGuard = qScopeGuard([&processToken]{
			if (processToken)
			{
				CloseHandle(processToken);
			}
		});
	if (!OpenProcessToken(processHandle, TOKEN_QUERY, &processToken))
	{
		qCWarning(rproxy) << "Cannot call TOKEN_QUERY:" << GetLastError();
		return QString();
	}

	DWORD dwSize = 0;
	if (!GetTokenInformation(processToken, TokenUser, nullptr, dwSize, &dwSize))
	{
		DWORD dwResult = GetLastError();
		if (dwResult != ERROR_INSUFFICIENT_BUFFER)
		{
			qCWarning(rproxy) << "GetTokenInformation failed:" << dwResult;
			return QString();
		}
	}

	PTOKEN_USER tokenUser = static_cast<PTOKEN_USER>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize));
	const auto tokenUserGuard = qScopeGuard([&tokenUser]{
			if (tokenUser)
			{
				HeapFree(GetProcessHeap(), 0, tokenUser);
			}
		});

	if (!GetTokenInformation(processToken, TokenUser, tokenUser, dwSize, &dwSize))
	{
		qCWarning(rproxy) << "GetTokenInformation failed:" << GetLastError();
		return QString();
	}

	DWORD nameLen = 255;
	LPTSTR name = static_cast<LPTSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nameLen));
	DWORD domainLen = 255;
	LPTSTR domain = static_cast<LPTSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, domainLen));
	const auto memGuard = qScopeGuard([&name, &domain] {
			HeapFree(GetProcessHeap(), 0, name);
			HeapFree(GetProcessHeap(), 0, domain);
		});

	SID_NAME_USE name_use;
	if (!LookupAccountSid(nullptr, tokenUser->User.Sid, name, &nameLen, domain, &domainLen, &name_use))
	{
		qCWarning(rproxy) << "LookupAccountSid failed:" << GetLastError();
		return QString();
	}

	return QString::fromWCharArray(name);
}


QString PortWrapper::getExecutableOfProcessID(DWORD pPid)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	const auto snapshotGuard = qScopeGuard([&snapshot]{
			if (snapshot)
			{
				CloseHandle(snapshot);
			}
		});

	if (!Process32First(snapshot, &entry))
	{
		qCWarning(rproxy) << "Process32First failed:" << GetLastError();
		return QString();
	}

	do
	{
		if (entry.th32ProcessID != pPid)
		{
			continue;
		}

		HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, false, entry.th32ProcessID);
		const auto processGuard = qScopeGuard([&process]{
				if (process)
				{
					CloseHandle(process);
				}
			});
		return QString::fromWCharArray(entry.szExeFile);
	}
	while (Process32Next(snapshot, &entry));

	return QString();
}


quint16 PortWrapper::getPortOfRunningProcess(const QList<MIB_TCPROW_OWNER_PID>& pConnections, const QString& pUser, quint16 pSelfPort, const in_addr& pProxyAddr)
{
	for (const auto& connection : pConnections)
	{
		if (connection.dwState != MIB_TCP_STATE_LISTEN
				|| connection.dwLocalAddr != pProxyAddr.S_un.S_addr)
		{
			continue;
		}

		if (getUserOfProcessID(connection.dwOwningPid) != pUser)
		{
			continue;
		}

		if (ntohs(static_cast<quint16>(connection.dwLocalPort)) == pSelfPort)
		{
			continue;
		}

		const auto& executable = getExecutableOfProcessID(connection.dwOwningPid);
		if (executable.isEmpty() || !QCoreApplication::applicationFilePath().endsWith(executable))
		{
			continue;
		}

		return ntohs(static_cast<quint16>(connection.dwLocalPort));
	}

	return 0;
}


QString PortWrapper::getUserOfConnection(const QList<MIB_TCPROW_OWNER_PID>& pConnections, quint16 pLocalPort, quint16 pRemotePort, const in_addr& pRemoteAddr)
{
	for (const auto& connection : pConnections)
	{
		if (connection.dwState != MIB_TCP_STATE_ESTAB
				|| connection.dwRemoteAddr != pRemoteAddr.S_un.S_addr
				|| connection.dwLocalAddr != pRemoteAddr.S_un.S_addr)
		{
			continue;
		}

		if (ntohs(static_cast<quint16>(connection.dwLocalPort)) == pRemotePort
				&& ntohs(static_cast<quint16>(connection.dwRemotePort)) == pLocalPort)
		{
			return getUserOfProcessID(connection.dwOwningPid);
		}
	}

	return QString();
}


QList<MIB_TCPROW_OWNER_PID> PortWrapper::getConnections()
{
	DWORD size = 0;
	DWORD dwResult = NO_ERROR;
	dwResult = GetExtendedTcpTable(nullptr, &size, false, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
	if (dwResult != ERROR_INSUFFICIENT_BUFFER)
	{
		qCWarning(rproxy) << "Cannot get IP table size";
		return QList<MIB_TCPROW_OWNER_PID>();
	}

	auto* tcpInfo = static_cast<MIB_TCPTABLE_OWNER_PID*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size));
	const auto tcpInfoGuard = qScopeGuard([tcpInfo]{
			HeapFree(GetProcessHeap(), 0, tcpInfo);
		});

	dwResult = GetExtendedTcpTable(tcpInfo, &size, false, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
	if (dwResult != NO_ERROR)
	{
		qCWarning(rproxy) << "Cannot get IP table:" << dwResult;
		return QList<MIB_TCPROW_OWNER_PID>();
	}

	QList<MIB_TCPROW_OWNER_PID> result;
	for (DWORD dwLoop = 0; dwLoop < tcpInfo->dwNumEntries; ++dwLoop)
	{
		result += tcpInfo->table[dwLoop];
	}
	return result;
}


quint16 PortWrapper::getProcessPort(quint16 pLocalPort, quint16 pRemotePort)
{
	struct in_addr localhost = {
		{
			{127, 0, 0, 1}
		}
	};
	const auto& connections = getConnections();
	const auto& user = getUserOfConnection(connections, pLocalPort, pRemotePort, localhost);
	if (user.isEmpty())
	{
		qCWarning(rproxy) << "Cannot detect user of connection";
		return 0;
	}
	qCDebug(rproxy) << "Detected user of request:" << user;
	return getPortOfRunningProcess(connections, user, pLocalPort, localhost);
}
