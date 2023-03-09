/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Windows specific implementation of the controller for retrieving and presenting diagnosis info.
 */


#include "DiagnosisController.h"

#include <objbase.h>
#include <setupapi.h>
#include <windows.h>

#include <QScopeGuard>

using namespace governikus;


#ifndef Q_OS_WINRT
static QString getWindowsDirectoryPath()
{
	const auto length = GetSystemWindowsDirectory(nullptr, 0);
	if (length > 0)
	{
		auto path = reinterpret_cast<LPWSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, length * sizeof(TCHAR)));
		if (path)
		{
			const auto guard = qScopeGuard([path] {
					HeapFree(GetProcessHeap(), 0, path);
				});
			if (GetSystemWindowsDirectory(path, length) > 0)
			{
				auto result = QString::fromWCharArray(path);
				return result;
			}

		}

	}

	return QStringLiteral("C:\\Windows");
}


static QString toAbsoluteWindowsDirectoryPath(const QString& pPath)
{
	if (pPath.contains(QLatin1Char(':')))
	{
		return pPath;
	}

	const QLatin1String systemRoot("\\SystemRoot");
	if (pPath.startsWith(systemRoot))
	{
		return getWindowsDirectoryPath() + pPath.mid(systemRoot.size());
	}

	return getWindowsDirectoryPath() + QLatin1Char('\\') + pPath;
}


static QString getWindowsFileVersionString(LPVOID pVersionData, const char* const pInfoName, int pLanguage, int pCodePage)
{
	const auto key = QStringLiteral("\\StringFileInfo\\%1%2\\%3")
			.arg(pLanguage, 4, 16, QLatin1Char('0'))
			.arg(pCodePage, 4, 16, QLatin1Char('0'))
			.arg(QString::fromLatin1(pInfoName));

	LPVOID version;
	UINT versionLength;
	if (!VerQueryValue(pVersionData, key.toStdWString().c_str(), &version, &versionLength))
	{
		return QString();
	}

	return QString::fromWCharArray(reinterpret_cast<wchar_t*>(version));
}


static void addWindowsComponentInfo(QVector<DiagnosisContext::ComponentInfo>& pComponents, const QString& pFileName)
{
	std::wstring fileName = pFileName.toStdWString();

	const auto infoSize = GetFileVersionInfoSize(fileName.data(), nullptr);
	if (infoSize == 0)
	{
		return;
	}

	LPVOID versionData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, infoSize);
	if (versionData == nullptr)
	{
		return;
	}
	const auto guard = qScopeGuard([versionData] {
			HeapFree(GetProcessHeap(), 0, versionData);
		});
	if (!GetFileVersionInfo(fileName.data(), 0, infoSize, versionData))
	{
		return;
	}

	struct
	{
		WORD mLanguage;
		WORD mCodePage;
	}
	* translateInfo;
	UINT translateInfoLength;

	if (!VerQueryValue(versionData, L"\\VarFileInfo\\Translation", reinterpret_cast<void**>(&translateInfo), &translateInfoLength))
	{
		return;
	}

	QString description;
	QString version;
	QString company;

	for (size_t i = 0; i < translateInfoLength / sizeof(*translateInfo); ++i)
	{
		if (description.isEmpty())
		{
			description = getWindowsFileVersionString(versionData, "FileDescription", translateInfo[i].mLanguage, translateInfo[i].mCodePage);
		}

		if (version.isEmpty())
		{
			version = getWindowsFileVersionString(versionData, "FileVersion", translateInfo[i].mLanguage, translateInfo[i].mCodePage);
		}

		if (company.isEmpty())
		{
			company = getWindowsFileVersionString(versionData, "CompanyName", translateInfo[i].mLanguage, translateInfo[i].mCodePage);
		}
	}

	if (description.isEmpty() && version.isEmpty() && company.isEmpty())
	{
		return;
	}

	pComponents += DiagnosisContext::ComponentInfo(pFileName, description, version, company);
}


static QSet<QString> getWindowsSmartCardDriverModuleNames()
{
	QSet<QString> moduleNames;

	// the GUID for the smart card reader device setup class
	wchar_t deviceClassGuidString[] = L"{50dd5230-ba8a-11d1-bf5d-0000f805f530}";
	GUID deviceClass;
	CLSIDFromString(deviceClassGuidString, &deviceClass);

	HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&deviceClass, nullptr, nullptr, 0);
	if (deviceInfoSet == INVALID_HANDLE_VALUE)
	{
		qDebug() << "SetupDiGetClassDevs() failed";
		return moduleNames;
	}

	SP_DEVINFO_DATA deviceInfoData;
	deviceInfoData.cbSize = sizeof(deviceInfoData);

	for (DWORD deviceIndex = 0; SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData); deviceIndex++)
	{
		if (SetupDiBuildDriverInfoList(deviceInfoSet, &deviceInfoData, SPDIT_COMPATDRIVER))
		{
			SP_DRVINFO_DATA_W driverInfoData;
			driverInfoData.cbSize = sizeof(driverInfoData);
			for (DWORD driverIndex = 0;
					SetupDiEnumDriverInfo(deviceInfoSet, &deviceInfoData, SPDIT_COMPATDRIVER, driverIndex, &driverInfoData);
					driverIndex++)
			{
				DWORD propertySize = 0;
				SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_SERVICE, nullptr, nullptr, 0, &propertySize);
				if (propertySize > 0)
				{
					auto serviceName = reinterpret_cast<LPWSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, propertySize));
					if (serviceName)
					{
						if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_SERVICE, nullptr,
								PBYTE(serviceName), propertySize, &propertySize))
						{
							moduleNames.insert(QString::fromWCharArray(serviceName));
						}
						HeapFree(GetProcessHeap(), 0, serviceName);
					}
				}
			}

			SetupDiDestroyDriverInfoList(deviceInfoSet, &deviceInfoData, SPDIT_COMPATDRIVER);
		}
	}

	SetupDiDestroyDeviceInfoList(deviceInfoSet);

	return moduleNames;
}


static QString getWindowsServiceDriverFileName(const QString& pServiceName)
{
	SC_HANDLE manager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
	if (manager == nullptr)
	{
		return QString();
	}

	QString path;

	SC_HANDLE service = OpenService(manager, pServiceName.toStdWString().c_str(), SC_MANAGER_ENUMERATE_SERVICE | SERVICE_QUERY_CONFIG);
	if (service != nullptr)
	{
		DWORD configSize = 0;
		QueryServiceConfig(service, nullptr, 0, &configSize);
		if (configSize > 0)
		{
			QUERY_SERVICE_CONFIG* serviceConfig = reinterpret_cast<QUERY_SERVICE_CONFIG*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, configSize));
			if (serviceConfig)
			{
				if (QueryServiceConfig(service, serviceConfig, configSize, &configSize))
				{
					path = toAbsoluteWindowsDirectoryPath(QString::fromWCharArray(serviceConfig->lpBinaryPathName));
				}

				HeapFree(GetProcessHeap(), 0, serviceConfig);
			}
		}

		CloseServiceHandle(service);
	}

	CloseServiceHandle(manager);

	return path;
}


#endif


void DiagnosisController::getPcscInfo(QVector<DiagnosisContext::ComponentInfo>& pComponents,
		QVector<DiagnosisContext::ComponentInfo>& pDrivers)
{
#ifndef Q_OS_WINRT
	addWindowsComponentInfo(pComponents, toAbsoluteWindowsDirectoryPath(QStringLiteral("System32\\WinSCard.dll")));
	addWindowsComponentInfo(pComponents, toAbsoluteWindowsDirectoryPath(QStringLiteral("System32\\SCardDlg.dll")));
	addWindowsComponentInfo(pComponents, toAbsoluteWindowsDirectoryPath(QStringLiteral("System32\\SCardSvr.dll")));
	addWindowsComponentInfo(pDrivers, toAbsoluteWindowsDirectoryPath(QStringLiteral("System32\\drivers\\smclib.sys")));

	const QSet<QString> moduleNames = getWindowsSmartCardDriverModuleNames();
	for (const QString& moduleName : moduleNames)
	{
		const QString path = getWindowsServiceDriverFileName(moduleName);
		addWindowsComponentInfo(pDrivers, path);
	}
#endif
}
