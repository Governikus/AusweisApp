/*!
 * \brief Unit tests for \ref CVCertificate
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QtCore>
#include <QtTest>

#include "asn1/ASN1Util.h"
#include "asn1/CVCertificate.h"
#include "asn1/KnownOIDs.h"
#include "TestFileHelper.h"


using namespace governikus;

class test_CVCertificate
	: public QObject
{
	Q_OBJECT

	static QByteArray valueOf(const BIGNUM* pBigNum)
	{
		if (pBigNum == nullptr)
		{
			return QByteArray();
		}
		int bufLen = BN_num_bytes(pBigNum);
		QVector<uchar> buf(bufLen);
		BN_bn2bin(pBigNum, buf.data());
		return QByteArray(reinterpret_cast<char*>(buf.data()), bufLen);
	}


	private Q_SLOTS:
		void getRawBody()
		{
			QByteArray body("7f4e82016e5f290100420e44454356434165494430303130337f4982011d060a04007f000702020202038120a9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef67530417affe7fb8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6bccdc18ff8c07b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3dac4fd97f8461a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3b561a6f7901e0e82974856a78641048925419fc7f194922cfc6b8dd25ae6a19c1b59216e6cf06270e5d75cfd64205f55cf867bbfefeefd6e680e1fd197f18ab684484901362568efc9adb5c6018d728701015f200e44454356434165494430303130337f4c12060904007f0007030102025305fc0f13ffff5f25060102010200035f2406010501020003");
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex");
			auto cvc = CVCertificate::fromHex(source);

			QCOMPARE(cvc->getRawBody(), QByteArray::fromHex(body));
		}


		void getRawSignature()
		{
			QByteArray signature("5f37406018cff8ee22ae417a63446a5fbf53ac39dd1ee331af4b62569604966f7575359b0c536673dcb98f9ae62c5f9724e0236bef6dc485ffa331be5e82de4410e2e8");
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex");
			auto cvc = CVCertificate::fromHex(source);

			QCOMPARE(cvc->getRawSignature(), QByteArray::fromHex(signature));
		}


		void toHex()
		{
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex");
			auto cvc = CVCertificate::fromHex(source);

			QCOMPARE(cvc->encode().toHex(), source);
		}


		void testParseAllCvcs()
		{
			for (auto name : {"cvat-DE0000024001HW.hex", "cvca-DECVCAeID00103.hex", "cvca-DETESTeID00005_DETESTeID00004.hex", "cvat-DEDEMODEV00038.hex", "cvca-DETESTeID00001.hex", "cvca-DETESTeID00005.hex", "cvat-DEDEMOPAA00079.hex", "cvca-DETESTeID00002_DETESTeID00001.hex",
							  "cvdv-DEDVeIDDPST00035.hex", "cvat-DEDEVDEMO00020.hex", "cvca-DETESTeID00002.hex", "cvdv-DEDVeIDDPST00039.hex", "cvca-DECVCAeID00102_DECVCAeID00103.hex", "cvca-DETESTeID00004_DETESTeID00002.hex", "cvdv-DEDVeIDDTR101415.hex",
							  "cvca-DECVCAeID00102.hex", "cvca-DETESTeID00004.hex", "cvdv-DEDVtIDGVNK00005.hex"}
					)
			{
				QByteArray bytes = TestFileHelper::readFile(QStringLiteral(":/card/") + QLatin1String(name));
				auto cvc = CVCertificate::fromHex(bytes);
				QVERIFY(cvc);
			}
		}


		void testIsValidOn()
		{
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex");
			auto cvc = CVCertificate::fromHex(source);

			QVERIFY(!cvc->isValidOn(QDateTime(QDate(2010, 8, 12), QTime(22, 59, 59, 999), Qt::OffsetFromUTC, -60 * 60 * 1)));
			QVERIFY(cvc->isValidOn(QDateTime(QDate(2010, 8, 13), QTime(23, 0, 0, 0), Qt::OffsetFromUTC, -60 * 60 * 1)) /*lower bound*/);
			QVERIFY(cvc->isValidOn(QDateTime(QDate(2012, 8, 13), QTime(23, 0, 0, 0), Qt::OffsetFromUTC, -60 * 60 * 1)) /*lower bound*/);
			QVERIFY(cvc->isValidOn(QDateTime(QDate(2013, 8, 14), QTime(2, 59, 59, 999), Qt::OffsetFromUTC, +60 * 60 * 3)) /*upper bound*/);
			QVERIFY(!cvc->isValidOn(QDateTime(QDate(2013, 8, 14), QTime(3, 0, 0, 0), Qt::OffsetFromUTC, +60 * 60 * 3)));
		}


		void testParseCrap()
		{
			QVERIFY(CVCertificate::fromHex(QByteArray("01020304060708")) == nullptr);
		}


		void testSuccess()
		{
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex");
			QVERIFY(CVCertificate::fromHex(source) != nullptr);
		}


		void testIsIssuedBy()
		{
			auto cvca1 = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"));
			auto cvcaLink2_1 = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00002_DETESTeID00001.hex"));

			QVERIFY(cvca1->isIssuedBy(*cvca1));
			QVERIFY(cvcaLink2_1->isIssuedBy(*cvca1));
			QVERIFY(!cvca1->isIssuedBy(*cvcaLink2_1));
		}


		void getEcdsaSignature()
		{
			auto cvca1 = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"));

			auto ecdsaSignature = cvca1->getEcdsaSignature();
			QCOMPARE(valueOf(ecdsaSignature->r).toHex(), QByteArray("9f25ebfaf4b91e4c60a1683754c5dc076a3179753ef97d9f8cb01fe1dcd3b8c8"));
			QCOMPARE(valueOf(ecdsaSignature->s).toHex(), QByteArray("3e7a26602ab1f344be5706006d79a9ff6a9716404dc83b9f30e1213b393128a2"));
		}


};

QTEST_GUILESS_MAIN(test_CVCertificate)
#include "test_CVCertificate.moc"
