/*!
 * \brief Unit tests for \ref TlsConfiguration
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "TlsConfiguration.h"

#include "AppSettings.h"
#include "MockNetworkReply.h"

#include <QtTest>


using namespace governikus;

class test_TlsConfiguration
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			AppSettings::getInstance().load();
		}


		void containsFatalError()
		{
			MockNetworkReply reply;
			QList<QSslError> errors;

			QVERIFY(!TlsConfiguration::containsFatalError(&reply, errors));

			errors.append(QSslError(QSslError::SslError::SelfSignedCertificate));
			QVERIFY(!TlsConfiguration::containsFatalError(&reply, errors));

			errors.append(QSslError(QSslError::SslError::SubjectIssuerMismatch));
			QVERIFY(TlsConfiguration::containsFatalError(&reply, errors));
		}


		void addUnsupportedCipher()
		{
			SslCipherList list;
			QVERIFY(list.isEmpty());
			list += QStringLiteral("dummy value");
			QVERIFY(list.isEmpty());
		}


		void addSupportedCipher()
		{
			SslCipherList list;
			QVERIFY(list.isEmpty());
			list += QStringLiteral("ECDHE-ECDSA-AES256-GCM-SHA384");
			QCOMPARE(list.length(), 1);
		}


		void addUnsupportedCurve()
		{
			SslEllipticCurveVector vector;
			QVERIFY(vector.isEmpty());
			vector += QStringLiteral("dummy value");
			QVERIFY(vector.isEmpty());
		}


		void addSupportedCurve()
		{
			SslEllipticCurveVector vector;
			QVERIFY(vector.isEmpty());
			vector += QStringLiteral("brainpoolP512r1");
			QCOMPARE(vector.length(), 1);
		}


};

QTEST_GUILESS_MAIN(test_TlsConfiguration)
#include "test_TlsConfiguration.moc"
