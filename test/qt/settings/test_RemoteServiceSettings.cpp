/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceSettings.h"

#include "DeviceInfo.h"
#include "KeyPair.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_RemoteServiceSettings
	: public QObject
{
	Q_OBJECT

	private:
		KeyPair pair1 = KeyPair::generate();
		KeyPair pair2 = KeyPair::generate();
		KeyPair pair3 = KeyPair::generate();

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
		}


		void testStandardValues()
		{
			RemoteServiceSettings settings;

			QCOMPARE(settings.getDeviceName(), DeviceInfo::getName());
		}


		void testDeviceName()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getDeviceName(), DeviceInfo::getName());

			QCOMPARE(settings.getDeviceName(), DeviceInfo::getName());

			settings.setDeviceName(QStringLiteral("   "));
			QCOMPARE(settings.getDeviceName(), DeviceInfo::getName());

			settings.setDeviceName(QStringLiteral("  \n "));
			QCOMPARE(settings.getDeviceName(), DeviceInfo::getName());

			settings.setDeviceName(QStringLiteral("Google Pixel"));
			QCOMPARE(settings.getDeviceName(), QLatin1String("Google Pixel"));

			settings.setDeviceName(QStringLiteral("   Google Pixel"));
			QCOMPARE(settings.getDeviceName(), QLatin1String("Google Pixel"));

			settings.setDeviceName(QStringLiteral("Google Pixel   "));
			QCOMPARE(settings.getDeviceName(), QLatin1String("Google Pixel"));

			settings.setDeviceName(QStringLiteral("Google Pixel  \n "));
			QCOMPARE(settings.getDeviceName(), QLatin1String("Google Pixel"));

			settings.setDeviceName(QStringLiteral("   "));
			QCOMPARE(settings.getDeviceName(), DeviceInfo::getName());

			settings.setDeviceName(QString());
			QCOMPARE(settings.getDeviceName(), DeviceInfo::getName());
		}


		void testPinPadMode()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getPinPadMode(), true);
			settings.setPinPadMode(false);
			QCOMPARE(settings.getPinPadMode(), false);
		}


		void testShowAccessRights()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getShowAccessRights(), false);
			settings.setPinPadMode(true);
			QCOMPARE(settings.getPinPadMode(), true);
		}


		void testDuplicatedTrustedCertificates()
		{
			RemoteServiceSettings settings;
			QVERIFY(settings.getTrustedCertificates().isEmpty());

			const QList<QSslCertificate> certs {
				pair1.getCertificate(), pair1.getCertificate(), pair2.getCertificate()
			};
			settings.setTrustedCertificates(certs);

			auto storedCerts = settings.getTrustedCertificates();
			QCOMPARE(storedCerts.size(), 2);
			QVERIFY(storedCerts.contains(pair1.getCertificate()));
			QVERIFY(storedCerts.contains(pair2.getCertificate()));

			settings.addTrustedCertificate(pair3.getCertificate());
			storedCerts = settings.getTrustedCertificates();
			QCOMPARE(storedCerts.size(), 3);
			QVERIFY(storedCerts.contains(pair1.getCertificate()));
			QVERIFY(storedCerts.contains(pair2.getCertificate()));
			QVERIFY(storedCerts.contains(pair3.getCertificate()));

			settings.addTrustedCertificate(pair3.getCertificate());
			QCOMPARE(storedCerts.size(), 3);
			QVERIFY(storedCerts.contains(pair1.getCertificate()));
			QVERIFY(storedCerts.contains(pair2.getCertificate()));
			QVERIFY(storedCerts.contains(pair3.getCertificate()));
		}


		void testTrustedCertificates()
		{
			RemoteServiceSettings settings;
			QVERIFY(settings.getTrustedCertificates().isEmpty());
			QList<QSslCertificate> certs;
			certs << pair3.getCertificate();
			certs << pair1.getCertificate();

			settings.setTrustedCertificates(certs);
			auto storedCerts = settings.getTrustedCertificates();
			QCOMPARE(settings.getTrustedCertificates().size(), 2);
			for (const auto& entry : std::as_const(certs))
			{
				QVERIFY(storedCerts.contains(entry));
			}

			certs.pop_front();
			QCOMPARE(storedCerts.size(), 2);

			QVERIFY(!storedCerts.contains(pair2.getCertificate()));
			settings.addTrustedCertificate(pair2.getCertificate());
			storedCerts = settings.getTrustedCertificates();
			QCOMPARE(storedCerts.size(), 3);

			certs << pair2.getCertificate();
			for (const auto& entry : std::as_const(certs))
			{
				QVERIFY(storedCerts.contains(entry));
			}

			settings.setTrustedCertificates({});
			QCOMPARE(settings.getTrustedCertificates().size(), 0);
		}


		void testRemoveTrustedCertificates()
		{
			RemoteServiceSettings settings;
			QVERIFY(settings.getTrustedCertificates().isEmpty());

			settings.addTrustedCertificate(pair1.getCertificate());
			settings.addTrustedCertificate(pair2.getCertificate());
			QCOMPARE(settings.getTrustedCertificates().size(), 2);

			const auto& fingerprint = pair1.getCertificate().digest(QCryptographicHash::Sha256);
			settings.removeTrustedCertificate(fingerprint);
			QCOMPARE(settings.getTrustedCertificates().size(), 1);
			QCOMPARE(settings.getTrustedCertificates().at(0), pair2.getCertificate());
		}


		void testCheckAndGenerateKey()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getKey(), QSslKey());
			QCOMPARE(settings.getCertificates().size(), 0);

			QVERIFY(settings.checkAndGenerateKey(2048));
			QCOMPARE(settings.getCertificates().size(), 1);
			const auto& key = settings.getKey();
			const auto cert = settings.getCertificates().at(0);
			QVERIFY(!key.isNull());
			QVERIFY(!cert.isNull());
			QVERIFY(cert.isSelfSigned());

			QVERIFY(settings.checkAndGenerateKey(2048));
			QCOMPARE(settings.getCertificates().size(), 1);
			QCOMPARE(settings.getKey(), key);
			QCOMPARE(settings.getCertificates().at(0), cert);

			QCOMPARE(cert.effectiveDate(), QDateTime::fromString(QStringLiteral("1970-01-01T00:00:00Z"), Qt::ISODate));
			QCOMPARE(cert.expiryDate(), QDateTime::fromString(QStringLiteral("9999-12-31T23:59:59Z"), Qt::ISODate));
			QVERIFY(cert.effectiveDate().isValid());
			QVERIFY(cert.expiryDate().isValid());

			QVERIFY(settings.checkAndGenerateKey(3072));
			QCOMPARE(settings.getCertificates().size(), 2);
			const auto derivedCert = settings.getCertificates().at(0);
			QVERIFY(settings.getKey() != key);
			QCOMPARE(settings.getCertificates().at(1), cert);
			QVERIFY(derivedCert != cert);

			QCOMPARE(derivedCert.issuerInfo(QSslCertificate::SerialNumber).size(), 1);
			QCOMPARE(cert.issuerInfo(QSslCertificate::SerialNumber).size(), 1);

			QCOMPARE(derivedCert.issuerInfo(QSslCertificate::SerialNumber).at(0),
					cert.issuerInfo(QSslCertificate::SerialNumber).at(0));

			QCOMPARE(derivedCert.issuerInfo(QSslCertificate::CommonName).size(), 1);
			QCOMPARE(cert.issuerInfo(QSslCertificate::CommonName).size(), 1);

			QCOMPARE(derivedCert.issuerInfo(QSslCertificate::CommonName).at(0),
					cert.issuerInfo(QSslCertificate::CommonName).at(0));

			QVERIFY(!derivedCert.isSelfSigned());

		}


		void testKey()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getKey(), QSslKey());
			QCOMPARE(settings.getKey(), QSslKey());

			settings.setKey(pair1.getKey());
			QCOMPARE(settings.getKey(), pair1.getKey());
			QVERIFY(!settings.getKey().isNull());
		}


		void testCertificate()
		{
			RemoteServiceSettings settings;
			QVERIFY(settings.getCertificates().isEmpty());
			QVERIFY(settings.getCertificates().isEmpty());

			settings.setCertificates({pair1.getCertificate()});
			QVERIFY(!settings.getCertificates().isEmpty());
			QCOMPARE(settings.getCertificates().constFirst(), pair1.getCertificate());
			QVERIFY(!settings.getCertificates().constFirst().isNull());
		}


		void testRemoteInfos()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getRemoteInfos().size(), 0);

			const auto& current = QDateTime::currentDateTime();
			RemoteServiceSettings::RemoteInfo a(QByteArrayLiteral("a"), current);
			a.setNameUnescaped(QStringLiteral("dummy for A"));
			RemoteServiceSettings::RemoteInfo b(QByteArrayLiteral("b"), current);
			b.setNameUnescaped(QStringLiteral("dummy for B"));

			QVERIFY(a == a);
			QVERIFY(b == b);
			QVERIFY(a != b);
			settings.setRemoteInfos({a, b});
			QCOMPARE(settings.getRemoteInfos().size(), 2);

			auto first = settings.getRemoteInfos().at(0);
			auto second = settings.getRemoteInfos().at(1);
			QCOMPARE(first, a);
			QCOMPARE(second, b);
			QCOMPARE(first.getFingerprint(), "a"_ba);
			QCOMPARE(first.getNameEscaped(), "dummy for A"_L1);
			QCOMPARE(first.getLastConnected(), current);
			QCOMPARE(second.getFingerprint(), "b"_ba);
			QCOMPARE(second.getNameEscaped(), "dummy for B"_L1);
			QCOMPARE(second.getLastConnected(), current);

			settings.setRemoteInfos({});
			QCOMPARE(settings.getRemoteInfos().size(), 0);

			settings.setRemoteInfos({a, b});
			QCOMPARE(settings.getRemoteInfos().size(), 2);
		}


		void testRemoteInfosUpdates()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getRemoteInfos().size(), 0);

			auto a = pair1.getCertificate();
			auto b = pair2.getCertificate();

			settings.addTrustedCertificate(a);
			settings.addTrustedCertificate(b);
			QCOMPARE(settings.getRemoteInfos().size(), 2);
			QCOMPARE(settings.getRemoteInfos().at(0).getNameEscaped(), QString());

			auto aInfo = settings.getRemoteInfo(a);
			aInfo.setNameUnescaped("dummy"_L1);
			QVERIFY(settings.updateRemoteInfo(aInfo));
			QCOMPARE(settings.getRemoteInfos().at(0).getNameEscaped(), "dummy"_L1);
			QCOMPARE(settings.getRemoteInfos().size(), 2);

			auto c = pair3.getCertificate();
			auto cInfo = settings.getRemoteInfo(c);
			QCOMPARE(cInfo.getFingerprint(), QByteArray());
			QCOMPARE(settings.getRemoteInfos().size(), 2);

			settings.addTrustedCertificate(c);
			cInfo = settings.getRemoteInfo(c);
			cInfo.setNameUnescaped("c"_L1);
			settings.updateRemoteInfo(cInfo);
			QCOMPARE(settings.getRemoteInfos().size(), 3);
			QCOMPARE(settings.getRemoteInfos().at(0).getNameEscaped(), "dummy"_L1);
			QCOMPARE(settings.getRemoteInfos().at(1).getNameEscaped(), QString());
			QCOMPARE(settings.getRemoteInfos().at(2).getNameEscaped(), "c"_L1);
			QCOMPARE(settings.getRemoteInfos().at(2).getFingerprint(),
					c.digest(QCryptographicHash::Sha256));
		}


		void testRemoteInfosSync()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getRemoteInfos().size(), 0);
			QVERIFY(!settings.updateRemoteInfo(RemoteServiceSettings::RemoteInfo()));

			auto a = pair1.getCertificate();
			settings.addTrustedCertificate(a);

			auto aInfo = settings.getRemoteInfo(a);
			aInfo.setNameUnescaped("<b>hallo</b>"_L1);
			QVERIFY(settings.updateRemoteInfo(aInfo));

			QCOMPARE(settings.getRemoteInfos().size(), 1);
			QCOMPARE(settings.getRemoteInfos().at(0).getNameEscaped(), QStringLiteral("&lt;b&gt;hallo&lt;/b&gt;"));
		}


		void testGenerateFingerprint()
		{
			QCOMPARE(RemoteServiceSettings::generateFingerprint(QSslCertificate()), QByteArray());
			auto cert = pair1.getCertificate();
			QVERIFY(!cert.isNull());
			const auto& fingerprint = cert.digest(QCryptographicHash::Sha256);
			QVERIFY(!fingerprint.isEmpty());
			QCOMPARE(RemoteServiceSettings::generateFingerprint(cert), fingerprint);
		}


};

QTEST_GUILESS_MAIN(test_RemoteServiceSettings)
#include "test_RemoteServiceSettings.moc"
