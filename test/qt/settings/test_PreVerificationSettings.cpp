/*!
 * \brief Unit tests for \ref PreVerificationSettings
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */
#include <QtTest>

#include "PreVerificationSettings.h"
#include "TestFileHelper.h"
#include "asn1/CVCertificate.h"


using namespace governikus;


class test_PreVerificationSettings
	: public QObject
{
	Q_OBJECT

	QByteArrayList cvcs;

	private Q_SLOTS:
		void initTestCase()
		{
			cvcs.append(TestFileHelper::readFile(":/card/cvca-DETESTeID00002_DETESTeID00001.hex"));
			cvcs.append(TestFileHelper::readFile(":/card/cvca-DETESTeID00004_DETESTeID00002.hex"));
			cvcs.append(TestFileHelper::readFile(":/card/cvdv-DEDVeIDDPST00035.hex"));
			cvcs.append(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"));
		}


		void init()
		{
			AbstractSettings::mTestDir.clear();
		}


		void testAdd()
		{
			PreVerificationSettings settings;

			QVERIFY(settings.addLinkCertificate(cvcs.at(0)));
			QVERIFY(settings.addLinkCertificate(cvcs.at(1)));
			QVERIFY(settings.addLinkCertificate(cvcs.at(2)));
			QVERIFY(settings.addLinkCertificate(cvcs.at(3)));
			QCOMPARE(settings.getLinkCertificates().size(), 4);
		}


		void testAddAlreadyContainedCert()
		{
			PreVerificationSettings settings;

			QVERIFY(settings.addLinkCertificate(cvcs.at(0)));
			QVERIFY(!settings.addLinkCertificate(cvcs.at(0)));
			QCOMPARE(settings.getLinkCertificates().size(), 1);
		}


		void testGetCerts()
		{
			PreVerificationSettings settings;
			settings.addLinkCertificate(cvcs.at(0));
			settings.addLinkCertificate(cvcs.at(1));
			settings.addLinkCertificate(cvcs.at(2));
			settings.addLinkCertificate(cvcs.at(3));

			QVERIFY(settings.getLinkCertificates().contains(cvcs.at(0)));
			QVERIFY(settings.getLinkCertificates().contains(cvcs.at(1)));
			QVERIFY(settings.getLinkCertificates().contains(cvcs.at(2)));
			QVERIFY(settings.getLinkCertificates().contains(cvcs.at(3)));
		}


		void testRemoveCert()
		{
			PreVerificationSettings settings;
			settings.addLinkCertificate(cvcs.at(0));
			settings.addLinkCertificate(cvcs.at(1));

			QVERIFY(settings.removeLinkCertificate(cvcs.at(0)));
			QCOMPARE(settings.getLinkCertificates().size(), 1);
		}


		void testRemoveAlreadyRemovedCert()
		{
			PreVerificationSettings settings;
			settings.addLinkCertificate(cvcs.at(0));
			settings.addLinkCertificate(cvcs.at(1));

			QVERIFY(settings.removeLinkCertificate(cvcs.at(0)));
			QVERIFY(!settings.removeLinkCertificate(cvcs.at(0)));
			QCOMPARE(settings.getLinkCertificates().size(), 1);
		}


		void testSaveLoad()
		{
			PreVerificationSettings settings;
			QVERIFY(settings.isEnabled());
			settings.save();

			settings.addLinkCertificate(cvcs.at(0));
			settings.addLinkCertificate(cvcs.at(1));
			settings.setEnabled(false);

			QVERIFY(settings.isUnsaved());
			settings.save();
			QVERIFY(!settings.isUnsaved());


			settings.setEnabled(true);
			for (const auto& cert : settings.getLinkCertificates())
			{
				settings.removeLinkCertificate(cert);
			}
			QVERIFY(settings.getLinkCertificates().isEmpty());

			settings.load();
			QCOMPARE(settings.getLinkCertificates().size(), 2);
			QVERIFY(settings.getLinkCertificates().contains(cvcs.at(0)));
			QVERIFY(settings.getLinkCertificates().contains(cvcs.at(1)));
			QVERIFY(!settings.isEnabled());
		}


		void testEnabled()
		{
			PreVerificationSettings settings;
			QVERIFY(settings.isEnabled());
			settings.load();
			QVERIFY(settings.isEnabled());
		}


		void testEquals()
		{
			PreVerificationSettings settings1;
			PreVerificationSettings settings2;

			QVERIFY(settings1 == settings2);

			settings1.addLinkCertificate(cvcs.at(0));
			settings2.addLinkCertificate(cvcs.at(3));

			QVERIFY(settings1 != settings2);

			settings1.addLinkCertificate(cvcs.at(1));
			settings2.addLinkCertificate(cvcs.at(2));

			QVERIFY(settings1 != settings2);

			settings1.addLinkCertificate(cvcs.at(2));
			settings2.addLinkCertificate(cvcs.at(1));

			QVERIFY(settings1 != settings2);

			settings1.addLinkCertificate(cvcs.at(3));
			settings2.addLinkCertificate(cvcs.at(0));

			QVERIFY(settings1 == settings2);
			QVERIFY(settings1 == settings1);
			QVERIFY(settings2 == settings2);

			settings1.setEnabled(false);
			QVERIFY(settings1 != settings2);
			settings2.setEnabled(false);
			QVERIFY(settings1 == settings2);
		}


};

QTEST_GUILESS_MAIN(test_PreVerificationSettings)
#include "test_PreVerificationSettings.moc"
