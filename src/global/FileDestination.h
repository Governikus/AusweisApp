/*
 * \brief Little helper that will abstract pathes of underlying systems
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCoreApplication>
#include <QStringBuilder>
#include <QLibraryInfo>

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

			#elif defined(Q_OS_LINUX) && defined(QT_NO_DEBUG)
			return QString("/usr/share/AusweisApp2");

			#else
			return QCoreApplication::applicationDirPath();

			#endif
		}


	public:
		static QString getPath(const QString& pFilename)
		{
			#if defined(Q_OS_LINUX) && defined(QT_NO_DEBUG)
			if (pFilename == QStringLiteral("translations"))
				return QLibraryInfo::location(QLibraryInfo::TranslationsPath);	
			else
			#endif	
				return getPath() % QLatin1Char('/') % pFilename;
		}


		static QString getPath(const char* pFilename)
		{
			return getPath() % QLatin1Char('/') % QString::fromUtf8(pFilename);
		}


};

} // namespace governikus
