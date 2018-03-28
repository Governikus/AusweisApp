/*!
 * \brief Unit tests for \ref RemoteHelper
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"
#include "RemoteHelper.h"
#include "RemoteServiceSettings.h"

#include <QtTest>

using namespace governikus;

class test_RemoteHelper
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testCheckAndGenerateKey()
		{
			auto& settings = AppSettings::getInstance().getRemoteServiceSettings();
			QCOMPARE(settings.getKey(), QSslKey());
			QCOMPARE(settings.getCertificate(), QSslCertificate());

			QVERIFY(RemoteHelper::checkAndGenerateKey());
			const auto& key = settings.getKey();
			const auto& cert = settings.getCertificate();
			QVERIFY(!key.isNull());
			QVERIFY(!cert.isNull());

			QVERIFY(RemoteHelper::checkAndGenerateKey());
			QCOMPARE(settings.getKey(), key);
			QCOMPARE(settings.getCertificate(), cert);

			QCOMPARE(settings.getCertificate().effectiveDate(), QDateTime::fromString(QStringLiteral("1970-01-01T00:00:00Z"), Qt::ISODate));
			QCOMPARE(settings.getCertificate().expiryDate(), QDateTime::fromString(QStringLiteral("9999-12-31T23:59:59Z"), Qt::ISODate));
			QVERIFY(settings.getCertificate().effectiveDate().isValid());
			QVERIFY(settings.getCertificate().expiryDate().isValid());
		}


};


QTEST_GUILESS_MAIN(test_RemoteHelper)
#include "test_RemoteHelper.moc"
