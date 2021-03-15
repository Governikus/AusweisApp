/*!
 * \brief Mac OS X specific implementation of the controller for retrieving and presenting diagnosis info.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */


#include "DiagnosisController.h"

#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>

#include <Cocoa/Cocoa.h>


using namespace governikus;


class PackageInfo
{
	public:
		QString mPackageId;
		QString mVersion;
		QString mVolume;
		QString mLocation;
		QString mInstallTime;

	public:
		PackageInfo(const QString& pData)
		{
			mPackageId = getField(pData, QStringLiteral("package-id"));
			mVersion = getField(pData, QStringLiteral("version"));
			mVolume = getField(pData, QStringLiteral("volume"));
			mLocation = getField(pData, QStringLiteral("location"));
			mInstallTime = getField(pData, QStringLiteral("install-time"));
		}

	private:
		QString getField(const QString& pData, const QString& pFieldName)
		{
			QString regex = QStringLiteral("\\b%1:([^\\n]*)\\n");
			const QRegularExpression regExp(regex.arg(pFieldName));
			QRegularExpressionMatch match = regExp.match(pData);
			if (!match.hasMatch())
			{
				return QString();
			}

			return match.captured(1).trimmed();
		}


};


class DriverInfo
{
	public:
		QString mBundlePath;
		QString mIfdManufacturer;
		QString mIfdFriendlyName;
		QString mIfdProductName;
		QString mBundleShortVersion;
		QString mBundleVersion;
		QString mBundleExecutable;
		QString mPackageVersion;

	public:
		DriverInfo()
		{
		}


		DriverInfo(const QString& pBundlePath, const QSettings& pSettings)
		{
			mBundlePath = pBundlePath;
			mIfdManufacturer = getField(pSettings, QStringLiteral("ifdManufacturerString"));
			mIfdFriendlyName = getField(pSettings, QStringLiteral("ifdFriendlyName"));
			mIfdProductName = getField(pSettings, QStringLiteral("ifdProductString"));
			mBundleShortVersion = getField(pSettings, QStringLiteral("CFBundleShortVersionString"));
			mBundleVersion = getField(pSettings, QStringLiteral("CFBundleVersion"));
			mBundleExecutable = getField(pSettings, QStringLiteral("CFBundleExecutable"));
		}


		bool isValid() const
		{
			return !mIfdManufacturer.isEmpty();
		}

	private:
		QString getField(const QSettings& pSettings, const QString& pFieldName)
		{
			return pSettings.value(pFieldName).toString();
		}


};


static void getDriverInfos(QHash<QString, DriverInfo>& pDriverInfos, const QString& pDirectoryPath)
{
	QDir dir(pDirectoryPath);
	const QStringList driverEntries = dir.entryList(QStringList({QStringLiteral("*.bundle")}));

	for (const QString& entry : driverEntries)
	{
		QString bundlePath = pDirectoryPath + QLatin1Char('/') + entry;
		QString plistPath = bundlePath + QStringLiteral("/Contents/Info.plist");
		QSettings settings(plistPath, QSettings::NativeFormat);
		DriverInfo driverInfo(bundlePath, settings);
		if (driverInfo.isValid())
		{
			pDriverInfos.insert(plistPath, driverInfo);
		}
	}
}


static QString runProcessAndReadAllOutput(const QString& pProgramm, const QStringList& pArguments)
{
	QString allOutput;
	QProcess process;
	process.start(pProgramm, pArguments);
	while (process.waitForReadyRead())
	{
		allOutput += QString::fromUtf8(process.readAllStandardOutput());
	}

	return allOutput;
}


void DiagnosisController::getPcscInfo(QVector<DiagnosisContext::ComponentInfo>& /*pComponents*/,
		QVector<DiagnosisContext::ComponentInfo>& pDrivers)
{
	QString driverDirectory = QStringLiteral("/usr/libexec/SmartCardServices/drivers");
	QString driverDirectoryLocal = QStringLiteral("/usr/local/libexec/SmartCardServices/drivers");

	QHash<QString, DriverInfo> driverInfos;

	getDriverInfos(driverInfos, driverDirectory);
	getDriverInfos(driverInfos, driverDirectoryLocal);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
	const auto split = Qt::SkipEmptyParts;
#else
	const auto split = QString::SkipEmptyParts;
#endif

	QString output = runProcessAndReadAllOutput(QStringLiteral("pkgutil"), QStringList({QStringLiteral("--pkgs")}));
	const QStringList packageIds = output.split(QLatin1Char('\n'), split);
	for (const QString& packageId : packageIds)
	{
		// skip Apple packages
		if (packageId.startsWith(QLatin1String("com.apple.pkg.")))
		{
			continue;
		}

		output = runProcessAndReadAllOutput(QStringLiteral("pkgutil"), QStringList({QStringLiteral("--info"), packageId}));
		PackageInfo packageInfo(output);

		if (packageInfo.mPackageId != packageId)
		{
			continue;
		}

		// get normalized location
		QString location = packageInfo.mVolume + QLatin1Char('/') + packageInfo.mLocation;
		location = QLatin1Char('/') + location.split(QLatin1Char('/'), split).join(QLatin1Char('/'));

		// skip packages that are definitely installed elsewhere
		if (!driverDirectory.startsWith(location) && !location.startsWith(driverDirectory))
		{
			continue;
		}

		// Get the file list for the package and check whether it contains any of the driver plist files.
		output = runProcessAndReadAllOutput(QStringLiteral("pkgutil"), QStringList({QStringLiteral("--files"), packageId}));
		const QStringList files = output.split(QLatin1Char('\n'), split);
		for (const QString& file : files)
		{
			if (!file.endsWith(QLatin1String("Info.plist")))
			{
				continue;
			}

			QString filePath = file.startsWith(QLatin1Char('/')) || location.endsWith(QLatin1Char('/'))
					? location + file : location + QLatin1Char('/') + file;

			if (driverInfos.contains(filePath))
			{
				driverInfos[filePath].mPackageVersion = packageInfo.mVersion;
				break;
			}
		}
	}

	// add the component infos
	for (const QString& driverPlist : driverInfos.keys())
	{
		const DriverInfo& driverInfo = driverInfos.value(driverPlist);

		QString driverPath = driverInfo.mBundlePath;
		if (!driverInfo.mBundleExecutable.isEmpty())
		{
			driverPath += QStringLiteral("/Contents/MacOS/");
			driverPath += driverInfo.mBundleExecutable;
		}

		QString description = !driverInfo.mIfdFriendlyName.isEmpty()
				? driverInfo.mIfdFriendlyName : driverInfo.mIfdProductName;

		QString version = driverInfo.mPackageVersion;
		if (version.isEmpty())
		{
			version = !driverInfo.mBundleShortVersion.isEmpty()
					? driverInfo.mBundleShortVersion : driverInfo.mBundleVersion;
		}

		pDrivers += DiagnosisContext::ComponentInfo(
				driverPath,
				description,
				version,
				driverInfo.mIfdManufacturer);
	}
}
