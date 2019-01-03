/*
 * \brief Helper to get build date and time.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QtGlobal>

#ifdef Q_OS_ANDROID
#include <QByteArrayList>
#endif

namespace governikus
{

class BuildHelper
{
	private:
		BuildHelper() = delete;
		~BuildHelper() = delete;

	public:
		static const char* getDateTime();


#ifdef Q_OS_ANDROID
		static int getVersionCode();
		static int getVersionCode(const QString& pPackageName);
		static QString getPackageName();
		static QByteArrayList getAppCertificates();
		static QByteArrayList getAppCertificates(const QString& pPackageName);
#endif


};

} // namespace governikus
