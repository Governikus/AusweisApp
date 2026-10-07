/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include <QCoreApplication>
#include <QList>
#include <QString>

#ifdef Q_OS_ANDROID
	#include <QByteArrayList>
	#include <QJniObject>
#endif

#include <functional>
#include <utility>

namespace governikus
{


class BuildHelper
{
	Q_DECLARE_TR_FUNCTIONS(BuildHelper)

	private:
		BuildHelper() = delete;
		~BuildHelper() = delete;

		[[nodiscard]] static bool fetchUserInteractive();

	public:
		static QList<std::pair<QLatin1String, QString>> getInformationHeader();
		static void processInformationHeader(const std::function<void(const QString&, const QString&)>& pFunc, bool pTranslate = true);

		[[nodiscard]] static bool isUserInteractive();


#ifdef Q_OS_ANDROID
		static QJniObject getPackageInfo(const QString& pPackageName, int pFlags = 0);
		static int getVersionCode();
		static int getVersionCode(const QString& pPackageName);
		static QString getPackageName();
		static QByteArrayList getAppCertificates(const QString& pPackageName);
#endif


};

} // namespace governikus
