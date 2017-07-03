/*
 * \brief Little helper that will abstract pathes of underlying systems
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QCoreApplication>
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
			#ifdef Q_OS_ANDROID
			return QStringLiteral("assets:");

			#else
			return QCoreApplication::applicationDirPath();

			#endif
		}


	public:
		static QString getPath(const QString& pFilename)
		{
			return getPath() % QLatin1Char('/') % pFilename;
		}


		static QString getPath(const char* pFilename)
		{
			return getPath() % QLatin1Char('/') % QString::fromUtf8(pFilename);
		}


};

} /* namespace governikus */
