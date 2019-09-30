/*!
 * \brief Unit tests for \ref TlsConfiguration
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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
			QCOMPARE(mTlsConfiguration.getCiphers().at(0), QSslCipher("ECDHE-ECDSA-AES256-GCM-SHA384"));
			QCOMPARE(mTlsConfiguration.getCiphers().at(1), QSslCipher("DHE-RSA-AES256-SHA256"));
		}


		void testLoadEllipticCurves()
		{
			QByteArray config("{"
							  "  \"ellipticCurves\":  [\"brainpoolP512r1\", \"brainpoolP384r1\"]"
							  "}");

			mTlsConfiguration.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsConfiguration.getEllipticCurves().size(), 2);
			QCOMPARE(mTlsConfiguration.getEllipticCurves().at(0), QSslEllipticCurve::fromLongName("brainpoolP512r1"));
			QCOMPARE(mTlsConfiguration.getEllipticCurves().at(1), QSslEllipticCurve::fromLongName("brainpoolP384r1"));
		}


		void testLoadSignatureAlgorithms()
		{
			#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
			QSKIP("SignatureAlgorithms not supported");
			#endif

			QByteArray config("{"
							  "  \"signatureAlgorithms\":  [\"RSA+SHA512\", \"DSA+SHA384\", \"ECDSA+SHA256\"]"
							  "}");

			mTlsConfiguration.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms().size(), 3);
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms().at(0), QByteArray("RSA+SHA512"));
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms().at(1), QByteArray("DSA+SHA384"));
			QCOMPARE(mTlsConfiguration.getSignatureAlgorithms().at(2), QByteArray("ECDSA+SHA256"));
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
									"  \"signatureAlgorithms\":  [\"Rsa+Sha512\", \"Dsa+Sha384\", \"Ec+Sha256\"]"
#endif
									"}"});

			for (int i = 0; i < configs.size(); ++i)
			{
				if (i > 0)
				{
					const auto& name1 = QStringLiteral("config%1 != config%2").arg(i - 1).arg(i).toLatin1();
					QTest::newRow(name1.data()) << configs.at(i - 1) << configs.at(i) << false;
				}
				const auto& name2 = QStringLiteral("config%1 == config%1").arg(i).toLatin1();
				QTest::newRow(name2.data()) << configs.at(i) << configs.at(i) << true;
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
