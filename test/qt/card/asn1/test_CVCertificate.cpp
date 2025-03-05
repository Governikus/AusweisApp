/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/CVCertificate.h"

#include "Converter.h"
#include "TestFileHelper.h"

#include <QtTest>
#include <openssl/bn.h>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_CVCertificate
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getRawBody()
		{
			QByteArray body("7f4e82016e5f290100420e44454356434165494430303130337f4982011d060a04007f000702020202038120a9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef67530417affe7fb8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6bccdc18ff8c07b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3dac4fd97f8461a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3b561a6f7901e0e82974856a78641048925419fc7f194922cfc6b8dd25ae6a19c1b59216e6cf06270e5d75cfd64205f55cf867bbfefeefd6e680e1fd197f18ab684484901362568efc9adb5c6018d728701015f200e44454356434165494430303130337f4c12060904007f0007030102025305fc0f13ffff5f25060102010200035f2406010501020003");
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex"_L1);
			auto cvc = Converter::certificatefromHex(source);

			QCOMPARE(cvc->getRawBody(), QByteArray::fromHex(body));
		}


		void getRawSignature()
		{
			QByteArray signature("5f37406018cff8ee22ae417a63446a5fbf53ac39dd1ee331af4b62569604966f7575359b0c536673dcb98f9ae62c5f9724e0236bef6dc485ffa331be5e82de4410e2e8");
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex"_L1);
			auto cvc = Converter::certificatefromHex(source);

			QCOMPARE(cvc->getRawSignature(), QByteArray::fromHex(signature));
		}


		void toHex()
		{
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex"_L1);
			auto cvc = Converter::certificatefromHex(source);

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
				auto cvc = Converter::certificatefromHex(bytes);
				QVERIFY(cvc);
			}
		}


		void testIsValidOn()
		{
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1);
			auto cvc = Converter::certificatefromHex(source);

			QVERIFY(!cvc->isValidOn(QDateTime(QDate(2010, 8, 12), QTime(22, 59, 59, 999), QTimeZone(-60 * 60 * 1))));
			QVERIFY(cvc->isValidOn(QDateTime(QDate(2010, 8, 13), QTime(23, 0, 0, 0), QTimeZone(-60 * 60 * 1))) /*lower bound*/);
			QVERIFY(cvc->isValidOn(QDateTime(QDate(2012, 8, 13), QTime(23, 0, 0, 0), QTimeZone(-60 * 60 * 1))) /*lower bound*/);
			QVERIFY(cvc->isValidOn(QDateTime(QDate(2013, 8, 14), QTime(2, 59, 59, 999), QTimeZone(+60 * 60 * 3))) /*upper bound*/);
			QVERIFY(!cvc->isValidOn(QDateTime(QDate(2013, 8, 14), QTime(3, 0, 0, 0), QTimeZone(+60 * 60 * 3))));
		}


		void testParseCrap()
		{
			QVERIFY(Converter::certificatefromHex(QByteArray("01020304060708")) == nullptr);
		}


		void testSuccess()
		{
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1);
			QVERIFY(Converter::certificatefromHex(source) != nullptr);
		}


		void testIsIssuedBy()
		{
			auto cvca1 = Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1));
			auto cvcaLink2_1 = Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00002_DETESTeID00001.hex"_L1));

			QVERIFY(cvca1->isIssuedBy(*cvca1));
			QVERIFY(cvcaLink2_1->isIssuedBy(*cvca1));
			QVERIFY(!cvca1->isIssuedBy(*cvcaLink2_1));
		}


		void getSignature()
		{
			QByteArray signature("9f25ebfaf4b91e4c60a1683754c5dc076a3179753ef97d9f8cb01fe1dcd3b8c83e7a26602ab1f344be5706006d79a9ff6a9716404dc83b9f30e1213b393128a2");
			auto cvca1 = Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1));

			QCOMPARE(cvca1->getSignature().toHex(), signature);
		}


		void debugStream()
		{
			const auto& regex = QStringLiteral(R"(CVC(type=CVCA, car="DETESTeID00001", chr="DETESTeID00001", valid=["2010-08-13","2013-08-13"])");
			const QRegularExpression output(QRegularExpression::escape(regex));

			QTest::ignoreMessage(QtDebugMsg, output);
			QSharedPointer<const CVCertificate> cvca = Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1));
			qDebug() << cvca;

			QTest::ignoreMessage(QtDebugMsg, output);
			QSharedPointer<CVCertificate> cvcsNonConst = qSharedPointerConstCast<CVCertificate>(cvca);
			qDebug() << cvcsNonConst;

			QTest::ignoreMessage(QtDebugMsg, output);
			QList<QSharedPointer<CVCertificate>> cvcsVector = {qSharedPointerConstCast<CVCertificate>(cvca)};
			qDebug() << cvcsVector;

			QTest::ignoreMessage(QtDebugMsg, output);
			QList<QSharedPointer<const CVCertificate>> cvcsVectorConst({cvca});
			qDebug() << cvcsVectorConst;
		}


		void equals()
		{
			QSharedPointer<const CVCertificate> cvca1_const = Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1));
			QSharedPointer<const CVCertificate> cvca2_const = Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1));
			QSharedPointer<CVCertificate> cvca1 = qSharedPointerConstCast<CVCertificate>(cvca1_const);
			QSharedPointer<CVCertificate> cvca2 = qSharedPointerConstCast<CVCertificate>(cvca2_const);

			QVERIFY(*cvca1 == *cvca2);
			QVERIFY(*cvca1_const == *cvca2_const);
			QVERIFY(*cvca1 == *cvca2_const);
			QVERIFY(*cvca2_const == *cvca1);
			QVERIFY(*cvca1_const == *cvca2);
			QVERIFY(*cvca2 == *cvca1_const);
		}


		void notEquals()
		{
			QSharedPointer<const CVCertificate> cvca1_const = Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1));
			QSharedPointer<const CVCertificate> cvca2_const = Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00002.hex"_L1));
			QSharedPointer<CVCertificate> cvca1 = qSharedPointerConstCast<CVCertificate>(cvca1_const);
			QSharedPointer<CVCertificate> cvca2 = qSharedPointerConstCast<CVCertificate>(cvca2_const);

			QVERIFY(*cvca1 != *cvca2);
			QVERIFY(*cvca1_const != *cvca2_const);
			QVERIFY(*cvca1 != *cvca2_const);
			QVERIFY(*cvca2_const != *cvca1);
			QVERIFY(*cvca1_const != *cvca2);
			QVERIFY(*cvca2 != *cvca1_const);
		}


};

QTEST_GUILESS_MAIN(test_CVCertificate)
#include "test_CVCertificate.moc"
