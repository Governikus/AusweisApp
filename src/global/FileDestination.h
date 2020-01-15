/*
 * \brief Little helper that will abstract pathes of underlying systems
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QStringBuilder>

namespace governikus
{

class FileDestination
{
	private:
		FileDestination() = delete;
		~FileDestination() = delete;
		Q_DISABLE_COPY(FileDestination)

		static QString getPath()
		{
			#if defined(Q_OS_ANDROID)
			return QStringLiteral("assets:");

			#elif defined(Q_OS_MACOS) && defined(QT_NO_DEBUG)
			return QCoreApplication::applicationDirPath() + QStringLiteral("/../Resources");

			#else
			return QCoreApplication::applicationDirPath();

			#endif
		}


	public:
		static QString getPath(const QString& pFilename,
			QStandardPaths::LocateOption pOption = QStandardPaths::LocateFile,
			QStandardPaths::StandardLocation pStandard = QStandardPaths::AppDataLocation)
		{
			#if defined(Q_OS_LINUX) || (defined(Q_OS_BSD4) && !defined(Q_OS_MACOS) && !defined(Q_OS_IOS))
			const auto match = QStandardPaths::locate(pStandard, pFilename, pOption);
			if (!match.isNull())
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
