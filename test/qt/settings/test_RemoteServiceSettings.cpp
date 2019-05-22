/*!
 * \brief Unit tests for \ref RemoteSertviceSettings
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceSettings.h"

#include "DeviceInfo.h"
#include "KeyPair.h"

#include <QtTest>

using namespace governikus;


class test_RemoteServiceSettings
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
		}


		void testStandardValues()
		{
			RemoteServiceSettings settings;

			QCOMPARE(settings.getServerName(), DeviceInfo::getName());
		}


		void testServerName()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getServerName(), DeviceInfo::getName());
			settings.save();
			QCOMPARE(settings.getServerName(), DeviceInfo::getName());
			settings.setServerName(QLatin1String("Google Pixel"));
			QCOMPARE(settings.getServerName(), QLatin1String("Google Pixel"));
		}


		void testPinPadMode()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getPinPadMode(), false);
			settings.save();
			QCOMPARE(settings.getPinPadMode(), false);
			settings.setPinPadMode(true);
			QCOMPARE(settings.getPinPadMode(), true);
		}


		void testDuplicatedTrustedCertificates()
		{
			const KeyPair pair1 = KeyPair::generate();
			const KeyPair pair2 = KeyPair::generate();
			const KeyPair pair3 = KeyPair::generate();

			RemoteServiceSettings settings;
			QVERIFY(settings.getTrustedCertificates().isEmpty());

			const QList<QSslCertificate> certs{
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
			const KeyPair pair1 = KeyPair::generate();

			RemoteServiceSettings settings;
			QVERIFY(settings.getTrustedCertificates().isEmpty());
			QList<QSslCertificate> certs;
			certs << QSslCertificate();
			certs << pair1.getCertificate();

			settings.setTrustedCertificates(certs);
			auto storedCerts = settings.getTrustedCertificates();
			QCOMPARE(settings.getTrustedCertificates().size(), 2);
			for (const auto& entry : qAsConst(certs))
			{
				QVERIFY(storedCerts.contains(entry));
			}

			certs.pop_front();
			QCOMPARE(storedCerts.size(), 2);

			const KeyPair pair2 = KeyPair::generate();
			QVERIFY(!storedCerts.contains(pair2.getCertificate()));
			settings.addTrustedCertificate(pair2.getCertificate());
			storedCerts = settings.getTrustedCertificates();
			QCOMPARE(storedCerts.size(), 3);

			certs << pair2.getCertificate();
			for (const auto& entry : qAsConst(certs))
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

			const KeyPair pair1 = KeyPair::generate();
			const KeyPair pair2 = KeyPair::generate();
			settings.addTrustedCertificate(pair1.getCertificate());
			settings.addTrustedCertificate(pair2.getCertificate());
			QCOMPARE(settings.getTrustedCertificates().size(), 2);

			const auto& fingerprint = QString::fromLatin1(pair1.getCertificate().digest(QCryptographicHash::Sha256).toHex());
			settings.removeTrustedCertificate(fingerprint);
			QCOMPARE(settings.getTrustedCertificates().size(), 1);
			QCOMPARE(settings.getTrustedCertificates().at(0), pair2.getCertificate());
		}


		void testCheckAndGenerateKey()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getKey(), QSslKey());
			QCOMPARE(settings.getCertificate(), QSslCertificate());

			QVERIFY(settings.checkAndGenerateKey());
			const auto& key = settings.getKey();
			const auto& cert = settings.getCertificate();
			QVERIFY(!key.isNull());
			QVERIFY(!cert.isNull());

			QVERIFY(settings.checkAndGenerateKey());
			QCOMPARE(settings.getKey(), key);
			QCOMPARE(settings.getCertificate(), cert);

			QCOMPARE(settings.getCertificate().effectiveDate(), QDateTime::fromString(QStringLiteral("1970-01-01T00:00:00Z"), Qt::ISODate));
			QCOMPARE(settings.getCertificate().expiryDate(), QDateTime::fromString(QStringLiteral("9999-12-31T23:59:59Z"), Qt::ISODate));
			QVERIFY(settings.getCertificate().effectiveDate().isValid());
			QVERIFY(settings.getCertificate().expiryDate().isValid());
		}


		void testKey()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getKey(), QSslKey());
			settings.save();
			QCOMPARE(settings.getKey(), QSslKey());

			const KeyPair pair = KeyPair::generate();
			settings.setKey(pair.getKey());
			QCOMPARE(settings.getKey(), pair.getKey());
			QVERIFY(!settings.getKey().isNull());
			settings.save();
		}


		void testCertificate()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getCertificate(), QSslCertificate());
			settings.save();
			QCOMPARE(settings.getCertificate(), QSslCertificate());

			const KeyPair pair = KeyPair::generate();
			settings.setCertificate(pair.getCertificate());
			QCOMPARE(settings.getCertificate(), pair.getCertificate());
			QVERIFY(!settings.getCertificate().isNull());
			settings.save();
		}


		void testRemoteInfos()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getRemoteInfos().size(), 0);

			const auto& current = QDateTime::currentDateTime();
			RemoteServiceSettings::RemoteInfo a(QString("a"), current, QString("dummy for A"));
			RemoteServiceSettings::RemoteInfo b(QString("b"), current, QString("dummy for B"));

			QVERIFY(a == a);
			QVERIFY(b == b);
			QVERIFY(a != b);
			settings.setRemoteInfos({a, b});
			QCOMPARE(settings.getRemoteInfos().size(), 2);

			auto first = settings.getRemoteInfos().at(0);
			auto second = settings.getRemoteInfos().at(1);
			QCOMPARE(first, a);
			QCOMPARE(second, b);
			QCOMPARE(first.getFingerprint(), QString("a"));
			QCOMPARE(first.getName(), QString("dummy for A"));
			QCOMPARE(first.getLastConnected(), current);
			QCOMPARE(second.getFingerprint(), QString("b"));
			QCOMPARE(second.getName(), QString("dummy for B"));
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

			auto a = KeyPair::generate().getCertificate();
			auto b = KeyPair::generate().getCertificate();

			settings.addTrustedCertificate(a);
			settings.addTrustedCertificate(b);
			QCOMPARE(settings.getRemoteInfos().size(), 2);
			QCOMPARE(settings.getRemoteInfos().at(0).getName(), QString());

			auto aInfo = settings.getRemoteInfo(a);
			aInfo.setName(QString("dummy"));
			QVERIFY(settings.updateRemoteInfo(aInfo));
			QCOMPARE(settings.getRemoteInfos().at(0).getName(), QString("dummy"));
			QCOMPARE(settings.getRemoteInfos().size(), 2);

			auto c = KeyPair::generate().getCertificate();
			auto cInfo = settings.getRemoteInfo(c);
			QCOMPARE(cInfo.getFingerprint(), QString());
			QCOMPARE(settings.getRemoteInfos().size(), 2);

			settings.addTrustedCertificate(c);
			cInfo = settings.getRemoteInfo(c);
			cInfo.setName("c");
			settings.updateRemoteInfo(cInfo);
			QCOMPARE(settings.getRemoteInfos().size(), 3);
			QCOMPARE(settings.getRemoteInfos().at(0).getName(), QString("dummy"));
			QCOMPARE(settings.getRemoteInfos().at(1).getName(), QString());
			QCOMPARE(settings.getRemoteInfos().at(2).getName(), QString("c"));
			QCOMPARE(settings.getRemoteInfos().at(2).getFingerprint(),
					QString::fromLatin1(c.digest(QCryptographicHash::Sha256).toHex()));
		}


		void testRemoteInfosSync()
		{
			RemoteServiceSettings settings;
			QCOMPARE(settings.getRemoteInfos().size(), 0);
			QVERIFY(!settings.updateRemoteInfo(RemoteServiceSettings::RemoteInfo()));

			auto a = KeyPair::generate().getCertificate();
			settings.addTrustedCertificate(a);

			auto aInfo = settings.getRemoteInfo(a);
			aInfo.setName("hallo");
			QVERIFY(settings.updateRemoteInfo(aInfo));

			QCOMPARE(settings.getRemoteInfos().size(), 1);
			QCOMPARE(settings.getRemoteInfos().at(0).getName(), QString("hallo"));
		}


		void testGenerateFingerprint()
		{
			QCOMPARE(RemoteServiceSettings::generateFingerprint(QSslCertificate()), QLatin1String());
			auto cert = KeyPair::generate().getCertificate();
			QVERIFY(!cert.isNull());
			const auto& fingerprint = QString::fromLatin1(cert.digest(QCryptographicHash::Sha256).toHex());
			QVERIFY(!fingerprint.isEmpty());
			QCOMPARE(RemoteServiceSettings::generateFingerprint(cert), fingerprint);
		}


};

QTEST_GUILESS_MAIN(test_RemoteServiceSettings)
#include "test_RemoteServiceSettings.moc"
