/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Helper to get build date and time.
 */

#pragma once

#include "EnumHelper.h"

#include <QCoreApplication>
#include <QPair>
#include <QString>
#include <QVector>

#ifdef Q_OS_ANDROID
	#include <QByteArrayList>
	#include <QJniObject>
#endif

#include <functional>

namespace governikus
{

defineEnumType(CertificateType
		, UNKNOWN
		, PRODUCTION
		, DEVELOPER
		)

class BuildHelper
{
	Q_DECLARE_TR_FUNCTIONS(BuildHelper)

	private:
		BuildHelper() = delete;
		~BuildHelper() = delete;

		[[nodiscard]] static CertificateType fetchCertificateType();
		[[nodiscard]] static bool fetchUserInteractive();

	public:
		static QVector<QPair<QLatin1String, QString>> getInformationHeader();
		static void processInformationHeader(const std::function<void(const QString&, const QString&)>& pFunc, bool pTranslate = true);

		[[nodiscard]] static CertificateType getCertificateType();
		[[nodiscard]] static bool isUserInteractive();


#ifdef Q_OS_ANDROID
		static QJniObject getPackageInfo(const QString& pPackageName, int pFlags = 0);
		static int getVersionCode();
		static int getVersionCode(const QString& pPackageName);
		static QString getPackageName();
		static QByteArrayList getAppCertificates();
		static QByteArrayList getAppCertificates(const QString& pPackageName);
#endif


};

} // namespace governikus
