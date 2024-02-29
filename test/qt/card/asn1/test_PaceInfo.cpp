/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref PACEInfo
 */

#include "asn1/PaceInfo.h"

#include "SecurityProtocol.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_PaceInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseCrap()
		{
			QVERIFY(PaceInfo::decode("3003020101") == nullptr);
		}


		void parseChipAuthenticationInfo()
		{
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202030202"
												   "            02 01 02"
												   "            02 01 08");

			QVERIFY(PaceInfo::decode(bytes) == nullptr);
		}


		void parsePaceInfoWithWrongContent()
		{
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040202"
												   "            04 01 02"
												   "            04 01 08");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo == nullptr);
		}


		void missingOid()
		{
			QByteArray bytes = QByteArray::fromHex("30 06"
												   "            02 01 02"
												   "            02 01 08");

			QVERIFY(PaceInfo::decode(bytes) == nullptr);
		}


		void missingVersion()
		{
			QByteArray bytes = QByteArray::fromHex("30 0C"
												   "            06 0A 04007F00070202040202");

			QVERIFY(PaceInfo::decode(bytes) == nullptr);
		}


		void withoutOptionalParameter()
		{
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QCOMPARE(paceInfo->getParameterId(), -1);
		}


		void getParameterId()
		{
			QByteArray bytes;
			QSharedPointer<const PaceInfo> paceInfo;

			bytes = QByteArray::fromHex("30 12"
										"            06 0A 04007F00070202040202"
										"            02 01 02"
										"            02 01 08");

			paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(paceInfo->hasParameterId());
			QCOMPARE(paceInfo->getParameterId(), 8);

			bytes = QByteArray::fromHex("30 0F"
										"            06 0A 04007F00070202040202"
										"            02 01 02");

			paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(!paceInfo->hasParameterId());
			QCOMPARE(paceInfo->getParameterId(), -1);
		}


		void getVersion()
		{
			QByteArray bytes;
			QSharedPointer<const PaceInfo> paceInfo;

			bytes = QByteArray::fromHex("30 12"
										"            06 0A 04007F00070202040202"
										"            02 01 01"
										"            02 01 08");

			paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QCOMPARE(paceInfo->getVersion(), 1);

			bytes = QByteArray::fromHex("30 12"
										"            06 0A 04007F00070202040202"
										"            02 01 02"
										"            02 01 08");

			paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QCOMPARE(paceInfo->getVersion(), 2);
		}


		void getEnumString()
		{
			QCOMPARE(getEnumName(KeyAgreementType::DH), QStringLiteral("DH"));
			QCOMPARE(getEnumName(MappingType::IM), QStringLiteral("IM"));
		}


		void getKeyAgreementType()
		{
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040101 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::DH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040102 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::DH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040103 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::DH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040104 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::DH);

			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040201 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::ECDH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040202 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::ECDH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040203 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::ECDH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040204 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::ECDH);

			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040301 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::DH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040302 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::DH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040303 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::DH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040304 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::DH);

			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040401 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::ECDH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040402 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::ECDH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040403 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::ECDH);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040404 020102"))->getProtocol().getKeyAgreement(), KeyAgreementType::ECDH);
		}


		void getMappingType()
		{
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040101 020102"))->getProtocol().getMapping(), MappingType::GM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040102 020102"))->getProtocol().getMapping(), MappingType::GM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040103 020102"))->getProtocol().getMapping(), MappingType::GM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040104 020102"))->getProtocol().getMapping(), MappingType::GM);

			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040201 020102"))->getProtocol().getMapping(), MappingType::GM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040202 020102"))->getProtocol().getMapping(), MappingType::GM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040203 020102"))->getProtocol().getMapping(), MappingType::GM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040204 020102"))->getProtocol().getMapping(), MappingType::GM);

			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040301 020102"))->getProtocol().getMapping(), MappingType::IM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040302 020102"))->getProtocol().getMapping(), MappingType::IM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040303 020102"))->getProtocol().getMapping(), MappingType::IM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040304 020102"))->getProtocol().getMapping(), MappingType::IM);

			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040401 020102"))->getProtocol().getMapping(), MappingType::IM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040402 020102"))->getProtocol().getMapping(), MappingType::IM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040403 020102"))->getProtocol().getMapping(), MappingType::IM);
			QCOMPARE(PaceInfo::decode(QByteArray::fromHex("300F 060A04007F00070202040404 020102"))->getProtocol().getMapping(), MappingType::IM);
		}


		void isStandardizedECDH_GM()
		{
			// version = 2, parameterId = 8
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02"
												   "            02 01 08");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(paceInfo->isStandardizedDomainParameters());
		}


		void isNotStandardizedECDH_GM()
		{
			// version = 2, parameterId = 7
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02"
												   "            02 01 07");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(!paceInfo->isStandardizedDomainParameters());
		}


		void isStandardizedECDH_IM()
		{
			// version = 2, parameterId = 8
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040402"
												   "            02 01 02"
												   "            02 01 08");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(paceInfo->isStandardizedDomainParameters());
		}


		void isNotStandardizedECDH_IM()
		{
			// version = 2, parameterId = 10
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040402"
												   "            02 01 02"
												   "            02 01 0A");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(!paceInfo->isStandardizedDomainParameters());
		}


		void isStandardizedDH_GM()
		{
			// version = 2, parameterId = 0
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040102"
												   "            02 01 02"
												   "            02 01 00");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(paceInfo->isStandardizedDomainParameters());
		}


		void isNotStandardizedDH_GM()
		{
			// version = 2, parameterId = 7
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040102"
												   "            02 01 02"
												   "            02 01 07");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(!paceInfo->isStandardizedDomainParameters());
		}


		void isStandardizedDH_IM()
		{
			// version = 2, parameterId = 0
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040302"
												   "            02 01 02"
												   "            02 01 00");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(paceInfo->isStandardizedDomainParameters());
		}


		void isNotStandardizedDH_IM()
		{
			// version = 2, parameterId = 8
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040302"
												   "            02 01 02"
												   "            02 01 08");

			auto paceInfo = PaceInfo::decode(bytes);

			QVERIFY(paceInfo != nullptr);
			QVERIFY(!paceInfo->isStandardizedDomainParameters());
		}


		void paramterId_data()
		{
			QTest::addColumn<int>("parameterId");
			QTest::addColumn<int>("curveNid");

			QTest::newRow("prime192") << 8 << NID_X9_62_prime192v1;
			QTest::newRow("brainpool192") << 9 << NID_brainpoolP192r1;
			QTest::newRow("secp224") << 10 << NID_secp224r1;
			QTest::newRow("brainpool224") << 11 << NID_brainpoolP224r1;
			QTest::newRow("prime256") << 12 << NID_X9_62_prime256v1;
			QTest::newRow("brainpool256") << 13 << NID_brainpoolP256r1;
			QTest::newRow("brainpool320") << 14 << NID_brainpoolP320r1;
			QTest::newRow("secp384") << 15 << NID_secp384r1;
			QTest::newRow("brainpool384") << 16 << NID_brainpoolP384r1;
			QTest::newRow("brainpool512") << 17 << NID_brainpoolP512r1;
			QTest::newRow("secp521") << 18 << NID_secp521r1;

			QTest::newRow("NID_undef 7") << 7 << NID_undef;
			QTest::newRow("NID_undef 19") << 19 << NID_undef;
		}


		void paramterId()
		{
			QFETCH(int, parameterId);
			QFETCH(int, curveNid);

			QCOMPARE(PaceInfo::getMappedNid(parameterId), curveNid);
		}


};

QTEST_GUILESS_MAIN(test_PaceInfo)
#include "test_PaceInfo.moc"
