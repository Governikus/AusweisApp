/*!
 * \brief Unit tests for \ref TlsConfiguration
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "TlsConfiguration.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_TlsConfiguration
	: public QObject
{
	Q_OBJECT
	TlsConfiguration mTlsConfiguration;

	private Q_SLOTS:
		void initTestCase()
		{
			mTlsConfiguration = TlsConfiguration();
		}


		void testDefaults()
		{
			QByteArray config("{}");

			mTlsConfiguration.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsConfiguration.getProtocolVersion(), QSsl::SecureProtocols);
			QCOMPARE(mTlsConfiguration.getCiphers().size(), 0);
			QCOMPARE(mTlsConfiguration.getEllipticCurves().size(), 0);
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms().size(), 0);
		}


		void testLoadprotocolVersion()
		{
			QByteArray config("{"
							  "  \"protocolVersion\": \"TlsV1_0OrLater\""
							  "}");

			mTlsConfiguration.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsConfiguration.getProtocolVersion(), QSsl::TlsV1_0OrLater);
		}


		void testLoadCiphers()
		{
			QByteArray config("{"
							  "  \"ciphers\": [\"ECDHE-ECDSA-AES256-GCM-SHA384\",\"DHE-RSA-AES256-SHA256\"]"
							  "}");

			mTlsConfiguration.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsConfiguration.getCiphers().size(), 2);
			QCOMPARE(mTlsConfiguration.getCiphers()[0], QSslCipher("ECDHE-ECDSA-AES256-GCM-SHA384"));
			QCOMPARE(mTlsConfiguration.getCiphers()[1], QSslCipher("DHE-RSA-AES256-SHA256"));
		}


		void testLoadEllipticCurves()
		{
			QByteArray config("{"
							  "  \"ellipticCurves\":  [\"brainpoolP512r1\", \"brainpoolP384r1\"]"
							  "}");

			mTlsConfiguration.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsConfiguration.getEllipticCurves().size(), 2);
			QCOMPARE(mTlsConfiguration.getEllipticCurves()[0], QSslEllipticCurve::fromLongName("brainpoolP512r1"));
			QCOMPARE(mTlsConfiguration.getEllipticCurves()[1], QSslEllipticCurve::fromLongName("brainpoolP384r1"));
		}


		void testLoadSignatureAlgorithms()
		{
			#ifndef GOVERNIKUS_QT
			QSKIP("SignatureAlgorithms not supported");
			#endif

			QByteArray config("{"
							  "  \"signatureAlgorithms\":  [\"Rsa+Sha512\", \"Dsa+Sha384\", \"Ec+Sha256\"]"
							  "}");

			mTlsConfiguration.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms().size(), 3);
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms()[0].first, QSsl::Rsa);
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms()[0].second, QCryptographicHash::Sha512);
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms()[1].first, QSsl::Dsa);
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms()[1].second, QCryptographicHash::Sha384);
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms()[2].first, QSsl::Ec);
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms()[2].second, QCryptographicHash::Sha256);
		}


		void testEquals_data()
		{
			QTest::addColumn<QByteArray>("config1");
			QTest::addColumn<QByteArray>("config2");
			QTest::addColumn<bool>("isEqual");

			QByteArrayList configs({"",
									"{"
									"  \"protocolVersion\": \"TlsV1_0OrLater\""
									"}",
									"{"
									"  \"protocolVersion\": \"TlsV1_0OrLater\","
									"  \"ciphers\": [\"ECDHE-ECDSA-AES256-GCM-SHA384\",\"DHE-RSA-AES256-SHA256\"]"
									"}",
									"{"
									"  \"protocolVersion\": \"TlsV1_0OrLater\","
									"  \"ciphers\": [\"ECDHE-ECDSA-AES256-GCM-SHA384\",\"DHE-RSA-AES256-SHA256\"],"
									"  \"ellipticCurves\":  [\"brainpoolP512r1\", \"brainpoolP384r1\"]"
									"}",
									"{"
									"  \"protocolVersion\": \"TlsV1_0OrLater\","
									"  \"ciphers\": [\"ECDHE-ECDSA-AES256-GCM-SHA384\",\"DHE-RSA-AES256-SHA256\"],"
									"  \"ellipticCurves\":  [\"brainpoolP512r1\", \"brainpoolP384r1\"],"
#ifdef GOVERNIKUS_QT
									"  \"signatureAlgorithms\":  [\"Rsa+Sha512\", \"Dsa+Sha384\", \"Ec+Sha256\"]"
#endif
									"}"});

			for (int i = 0; i < configs.size(); ++i)
			{
				if (i > 0)
				{
					const auto& name1 = QStringLiteral("config%1 != config%2").arg(i - 1).arg(i).toLatin1();
					QTest::newRow(name1.data()) << configs[i - 1] << configs[i] << false;
				}
				const auto& name2 = QStringLiteral("config%1 == config%1").arg(i).toLatin1();
				QTest::newRow(name2.data()) << configs[i] << configs[i] << true;
			}
		}


		void testEquals()
		{
			QFETCH(QByteArray, config1);
			QFETCH(QByteArray, config2);
			QFETCH(bool, isEqual);

			TlsConfiguration settings1, settings2;
			settings1.load(QJsonDocument::fromJson(config1).object());
			settings2.load(QJsonDocument::fromJson(config2).object());

			QCOMPARE(settings1 == settings2, isEqual);
		}


};

QTEST_GUILESS_MAIN(test_TlsConfiguration)
#include "test_TlsConfiguration.moc"
