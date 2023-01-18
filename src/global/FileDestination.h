/*
 * \brief Little helper that will abstract pathes of underlying systems
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QStringBuilder>

namespace governikus
{

class FileDestination
{
	Q_DISABLE_COPY(FileDestination)

	private:
		FileDestination() = delete;
		~FileDestination() = delete;

		static QString getPath()
		{
#if defined(Q_OS_ANDROID)
			return QStringLiteral("assets:");

#elif defined(Q_OS_MACOS)
			const auto& path = QCoreApplication::applicationDirPath() + QStringLiteral("/../Resources");

	#if !defined(QT_NO_DEBUG)
			if (!QFile::exists(path))
			{
				return QCoreApplication::applicationDirPath();
			}
	#endif

			return path;

#else
			return QCoreApplication::applicationDirPath();

#endif
		}

	public:
		static QString getPath(const QString& pFilename,
			QStandardPaths::LocateOption pOption = QStandardPaths::LocateFile,
			QStandardPaths::StandardLocation pStandard = QStandardPaths::AppDataLocation)
		{
#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) || (defined(Q_OS_BSD4) && !defined(Q_OS_MACOS) && !defined(Q_OS_IOS))
			if (const auto& match = QStandardPaths::locate(pStandard, pFilename, pOption); !match.isNull())
			{
				return match;
			}

			qDebug() << pFilename << "not found in following destinations |" << pOption;
			const auto defaultLocations = QStandardPaths::standardLocations(pStandard);
			for (const auto& location : defaultLocations)
			{
				qDebug() << location;
			}
#else
			Q_UNUSED(pOption)
			Q_UNUSED(pStandard)
#endif

			return getPath() % QLatin1Char('/') % pFilename;
		}


};

} // namespace governikus
