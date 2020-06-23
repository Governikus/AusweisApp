/*
 * \brief Helper to get build date and time.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCoreApplication>
#include <QPair>
#include <QString>
#include <QVector>

#ifdef Q_OS_ANDROID
#include <QByteArrayList>
#endif

#include <functional>

namespace governikus
{

class BuildHelper
{
	Q_DECLARE_TR_FUNCTIONS(BuildHelper)

	private:
		BuildHelper() = delete;
		~BuildHelper() = delete;

	public:
		static QVector<QPair<QLatin1String, QString>> getInformationHeader();
		static void processInformationHeader(const std::function<void(const QString&, const QString&)>& pFunc, bool pTranslate = true);


#ifdef Q_OS_ANDROID
		static int getVersionCode();
		static int getVersionCode(const QString& pPackageName);
		static QString getPackageName();
		static QByteArrayList getAppCertificates();
		static QByteArrayList getAppCertificates(const QString& pPackageName);
#endif


};

} // namespace governikus
