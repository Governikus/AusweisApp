/*!
 * \brief Unit tests for \ref TlsSettings
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "TlsSettings.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_TlsSettings
	: public QObject
{
	Q_OBJECT
	TlsSettings mTlsSettings;

	private Q_SLOTS:
		void initTestCase()
		{
			mTlsSettings = TlsSettings();
		}


		void testDefaults()
		{
			QByteArray config("{}");

			mTlsSettings.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsSettings.getProtocolVersion(), QSsl::SecureProtocols);
			QCOMPARE(mTlsSettings.getCiphers().size(), 0);
			QCOMPARE(mTlsSettings.getEllipticCurves().size(), 0);
			QCOMPARE(mTlsSettings.getSignatureAlgorithms().size(), 0);
		}


		void testLoadprotocolVersion()
		{
			QByteArray config("{"
							  "  \"protocolVersion\": \"TlsV1_0OrLater\""
							  "}");

			mTlsSettings.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsSettings.getProtocolVersion(), QSsl::TlsV1_0OrLater);
		}


		void testLoadCiphers()
		{
			QByteArray config("{"
							  "  \"ciphers\": [\"ECDHE-ECDSA-AES256-GCM-SHA384\",\"DHE-RSA-AES256-SHA256\"]"
							  "}");

			mTlsSettings.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsSettings.getCiphers().size(), 2);
			QCOMPARE(mTlsSettings.getCiphers()[0], QSslCipher("ECDHE-ECDSA-AES256-GCM-SHA384"));
			QCOMPARE(mTlsSettings.getCiphers()[1], QSslCipher("DHE-RSA-AES256-SHA256"));
		}


		void testLoadEllipticCurves()
		{
			QByteArray config("{"
							  "  \"ellipticCurves\":  [\"brainpoolP512r1\", \"brainpoolP384r1\"]"
							  "}");

			mTlsSettings.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsSettings.getEllipticCurves().size(), 2);
			QCOMPARE(mTlsSettings.getEllipticCurves()[0], QSslEllipticCurve::fromLongName("brainpoolP512r1"));
			QCOMPARE(mTlsSettings.getEllipticCurves()[1], QSslEllipticCurve::fromLongName("brainpoolP384r1"));
		}


		void testLoadSignatureAlgorithms()
		{
			QByteArray config("{"
							  "  \"signatureAlgorithms\":  [\"Rsa+Sha512\", \"Dsa+Sha384\", \"Ec+Sha256\"]"
							  "}");

			mTlsSettings.load(QJsonDocument::fromJson(config).object());

			QCOMPARE(mTlsSettings.getSignatureAlgorithms().size(), 3);
			QCOMPARE(mTlsSettings.getSignatureAlgorithms()[0].first, QSsl::Rsa);
			QCOMPARE(mTlsSettings.getSignatureAlgorithms()[0].second, QCryptographicHash::Sha512);
			QCOMPARE(mTlsSettings.getSignatureAlgorithms()[1].first, QSsl::Dsa);
			QCOMPARE(mTlsSettings.getSignatureAlgorithms()[1].second, QCryptographicHash::Sha384);
			QCOMPARE(mTlsSettings.getSignatureAlgorithms()[2].first, QSsl::Ec);
			QCOMPARE(mTlsSettings.getSignatureAlgorithms()[2].second, QCryptographicHash::Sha256);
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
									"  \"signatureAlgorithms\":  [\"Rsa+Sha512\", \"Dsa+Sha384\", \"Ec+Sha256\"]"
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

			TlsSettings settings1, settings2;
			settings1.load(QJsonDocument::fromJson(config1).object());
			settings2.load(QJsonDocument::fromJson(config2).object());

			QCOMPARE(settings1 == settings2, isEqual);
		}


};

QTEST_GUILESS_MAIN(test_TlsSettings)
#include "test_TlsSettings.moc"
