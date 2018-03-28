/*!
 * \brief Unit tests for \ref PreVerificationSettings
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "PreVerificationSettings.h"

#include "asn1/CVCertificate.h"
#include "TestFileHelper.h"

#include <QFile>
#include <QtTest>


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

			settings.addLinkCertificate(cvcs.at(0));
			settings.addLinkCertificate(cvcs.at(1));
			settings.addLinkCertificate(cvcs.at(2));
			settings.addLinkCertificate(cvcs.at(3));
			QCOMPARE(settings.getLinkCertificates().size(), 4);
			settings.save();

			QFile testFile(settings.mStore->fileName());
			QVERIFY(testFile.exists());
			QVERIFY(testFile.open(QIODevice::ReadOnly | QIODevice::Text));
			QCOMPARE(testFile.readAll(), QByteArray("[preverification]\nlinkcertificates\\1\\linkcertificate=@ByteArray(7f218201b67f4e82016e5f290100420e44455445535465494430303030317f4982011d060a04007f000702020202038120a9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef67530417affe7fb8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6bccdc18ff8c07b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3dac4fd97f8461a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3b561a6f7901e0e82974856a7864104096eb58bfd86252238ec2652185c43c3a56c320681a21e37a8e69ddc387c0c5f5513856efe2fdc656e604893212e29449b365e304605ac5413e75be31e641f128701015f200e44455445535465494430303030327f4c12060904007f0007030102025305fe0f01ffff5f25060100000902015f24060103000902015f3740141120a0fdfc011a52f3f72b387a3dc7aca88b4868d5ae9741780b6ff8a0b49e5f55169a2d298ef5cf95935dca0c3df3e9d42dc45f74f2066317154961e6c746)\nlinkcertificates\\2\\linkcertificate=@ByteArray(7f218201b67f4e82016e5f290100420e44455445535465494430303030327f4982011d060a04007f000702020202038120a9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef67530417affe7fb8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6bccdc18ff8c07b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3dac4fd97f8461a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3b561a6f7901e0e82974856a786410474ff63ab838c73c303ac003dfee95cf8bf55f91e8febcb7395d942036e47cf1845ec786ec95bb453aac288ad023b6067913cf9b63f908f49304e5cfc8b3050dd8701015f200e44455445535465494430303030347f4c12060904007f0007030102025305fc0f13ffff5f25060102000501015f24060105000501015f37405c035a0611b6c58f0b5261fdd009decab7dc7a79482d5248cca119059b7d82b2157cf0c4a499bcf441efdd35e294a58c0af19a34a0762159533285acf170a505)\nlinkcertificates\\3\\linkcertificate=@ByteArray(7F2181E77F4E81A05F290100420E44455445535465494430303030347F494F060A04007F000702020202038641045B1CB1090D5064FE0AEE21BD95C062AB94C952F7D64274EAF004C2E3DA4ABFDA2D0108B2545CEAF8BAF40BDEA1D161BE8950B3353BFD267F0674EC9ACABA71D05F2010444544566549444450535430303033357F4C12060904007F0007030102025305400513FF875F25060104000201015F24060104000501025F37409389856B8DA9956BA3E9894812BA87F866646660557131EF618349BF145A0826A8FB4A9BE22589CDE868B074C3FDA73DB84F9FDC84B87B3896702E42B4FE86E7)\nlinkcertificates\\4\\linkcertificate=@ByteArray(7F218201487F4E8201005F2901004210444544566549444450535430303033357F494F060A04007F0007020202020386410400C7DEDB2117E45ACF998E9D3ED34883E0617D1614B60430CA1DF1D2ECC96BC214D97451588EF706DEAF7F68163F7C8EAADF9EA028F0F8BF5D0DD67B650907175F200E444544454D4F44455630303033387F4C12060904007F0007030102025305000513FF875F25060104000301035F2406010400040101655E732D060904007F000703010301802012CA9D0A51DF9297EABA7EBE9AB49DF2F4CF83E0DBB02772EFAD89C8AD75FCCD732D060904007F0007030103028020CB1E1940159F11DC96845B87C23B86F9BAA755A789A914BBD5B8FA9784019D1C5F37407AB2B3C8DE4B3F7136F7DA91CCAC25B26AEC5BC35AD0B603FA2FFE50CEA49F785614AD3FB2EFF1719971FBCABC95F95A9F601F9017BD73952B45E645B90B774F)\nlinkcertificates\\size=4\n"));
		}


		void testAddAlreadyContainedCert()
		{
			PreVerificationSettings settings;

			settings.addLinkCertificate(cvcs.at(0));
			settings.addLinkCertificate(cvcs.at(0));
			QCOMPARE(settings.getLinkCertificates().size(), 1);
			settings.save();

			QFile testFile(settings.mStore->fileName());
			QVERIFY(testFile.exists());
			QVERIFY(testFile.open(QIODevice::ReadOnly | QIODevice::Text));
			QCOMPARE(testFile.readAll(), QByteArray("[preverification]\nlinkcertificates\\1\\linkcertificate=@ByteArray(7f218201b67f4e82016e5f290100420e44455445535465494430303030317f4982011d060a04007f000702020202038120a9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef67530417affe7fb8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6bccdc18ff8c07b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3dac4fd97f8461a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3b561a6f7901e0e82974856a7864104096eb58bfd86252238ec2652185c43c3a56c320681a21e37a8e69ddc387c0c5f5513856efe2fdc656e604893212e29449b365e304605ac5413e75be31e641f128701015f200e44455445535465494430303030327f4c12060904007f0007030102025305fe0f01ffff5f25060100000902015f24060103000902015f3740141120a0fdfc011a52f3f72b387a3dc7aca88b4868d5ae9741780b6ff8a0b49e5f55169a2d298ef5cf95935dca0c3df3e9d42dc45f74f2066317154961e6c746)\nlinkcertificates\\size=1\n"));
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
			settings.save();

			settings.removeLinkCertificate(cvcs.at(0));
			QCOMPARE(settings.getLinkCertificates().size(), 1);
			settings.save();

			QFile testFile(settings.mStore->fileName());
			QVERIFY(testFile.exists());
			QVERIFY(testFile.open(QIODevice::ReadOnly | QIODevice::Text));
			QCOMPARE(testFile.readAll(), QByteArray("[preverification]\nlinkcertificates\\1\\linkcertificate=@ByteArray(7f218201b67f4e82016e5f290100420e44455445535465494430303030327f4982011d060a04007f000702020202038120a9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef67530417affe7fb8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6bccdc18ff8c07b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3dac4fd97f8461a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3b561a6f7901e0e82974856a786410474ff63ab838c73c303ac003dfee95cf8bf55f91e8febcb7395d942036e47cf1845ec786ec95bb453aac288ad023b6067913cf9b63f908f49304e5cfc8b3050dd8701015f200e44455445535465494430303030347f4c12060904007f0007030102025305fc0f13ffff5f25060102000501015f24060105000501015f37405c035a0611b6c58f0b5261fdd009decab7dc7a79482d5248cca119059b7d82b2157cf0c4a499bcf441efdd35e294a58c0af19a34a0762159533285acf170a505)\nlinkcertificates\\size=1\n"));
		}


		void testRemoveAlreadyRemovedCert()
		{
			PreVerificationSettings settings;
			settings.addLinkCertificate(cvcs.at(0));
			settings.addLinkCertificate(cvcs.at(1));

			settings.removeLinkCertificate(cvcs.at(0));
			settings.removeLinkCertificate(cvcs.at(0));
			QCOMPARE(settings.getLinkCertificates().size(), 1);
		}


		void testEnabled()
		{
			PreVerificationSettings settings;
			QVERIFY(settings.isEnabled());
			settings.setEnabled(false);
			QVERIFY(!settings.isEnabled());
		}


};

QTEST_GUILESS_MAIN(test_PreVerificationSettings)
#include "test_PreVerificationSettings.moc"
