/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SecurityProtocol.h"

#include "LogHandler.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(Oid)


class test_SecurityProtocol
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void knownOIDs_data()
		{
			QTest::addColumn<Oid>("oid");
			QTest::addColumn<ProtocolType>("protocol");
			QTest::addColumn<KeyAgreementType>("keyAgreement");
			QTest::addColumn<MappingType>("mapping");
			QTest::addColumn<int>("keySize");
			QTest::addColumn<SignatureType>("signature");
			QTest::addColumn<QCryptographicHash::Algorithm>("hash");
			QTest::addColumn<bool>("causeWarnings");

			QTest::newRow("id-TA-RSA-v1-5-SHA-1") << Oid(KnownOid::ID_TA_RSA_V1_5_SHA_1) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::RSA << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-TA-RSA-v1-5-SHA-256") << Oid(KnownOid::ID_TA_RSA_V1_5_SHA_256) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::RSA << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-TA-RSA-PSS-SHA-1") << Oid(KnownOid::ID_TA_RSA_PSS_SHA_1) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::RSA << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-TA-RSA-PSS-SHA-256") << Oid(KnownOid::ID_TA_RSA_PSS_SHA_256) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::RSA << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-TA-RSA-v1-5-SHA-512") << Oid(KnownOid::ID_TA_RSA_V1_5_SHA_512) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::RSA << QCryptographicHash::Sha512 << false;
			QTest::newRow("id-TA-RSA-PSS-SHA-512") << Oid(KnownOid::ID_TA_RSA_PSS_SHA_512) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::RSA << QCryptographicHash::Sha512 << false;

			QTest::newRow("id-TA-ECDSA-SHA-1") << Oid(KnownOid::ID_TA_ECDSA_SHA_1) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::ECDSA << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-TA-ECDSA-SHA-224") << Oid(KnownOid::ID_TA_ECDSA_SHA_224) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::ECDSA << QCryptographicHash::Sha224 << false;
			QTest::newRow("id-TA-ECDSA-SHA-256") << Oid(KnownOid::ID_TA_ECDSA_SHA_256) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::ECDSA << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-TA-ECDSA-SHA-384") << Oid(KnownOid::ID_TA_ECDSA_SHA_384) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::ECDSA << QCryptographicHash::Sha384 << false;
			QTest::newRow("id-TA-ECDSA-SHA-512") << Oid(KnownOid::ID_TA_ECDSA_SHA_512) << ProtocolType::TA << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::ECDSA << QCryptographicHash::Sha512 << false;

			QTest::newRow("id-CA-DH-3DES-CBC-CBC") << Oid(KnownOid::ID_CA_DH_3DES_CBC_CBC) << ProtocolType::CA << KeyAgreementType::DH << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("id-CA-DH-AES-CBC-CMAC-128") << Oid(KnownOid::ID_CA_DH_AES_CBC_CMAC_128) << ProtocolType::CA << KeyAgreementType::DH << MappingType::UNDEFINED << 16 << SignatureType::UNDEFINED << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-CA-DH-AES-CBC-CMAC-192") << Oid(KnownOid::ID_CA_DH_AES_CBC_CMAC_192) << ProtocolType::CA << KeyAgreementType::DH << MappingType::UNDEFINED << 24 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-CA-DH-AES-CBC-CMAC-256") << Oid(KnownOid::ID_CA_DH_AES_CBC_CMAC_256) << ProtocolType::CA << KeyAgreementType::DH << MappingType::UNDEFINED << 32 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;

			QTest::newRow("id-CA-ECDH-3DES-CBC-CBC") << Oid(KnownOid::ID_CA_ECDH_3DES_CBC_CBC) << ProtocolType::CA << KeyAgreementType::ECDH << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("id-CA-ECDH-AES-CBC-CMAC-128") << Oid(KnownOid::ID_CA_ECDH_AES_CBC_CMAC_128) << ProtocolType::CA << KeyAgreementType::ECDH << MappingType::UNDEFINED << 16 << SignatureType::UNDEFINED << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-CA-ECDH-AES-CBC-CMAC-192") << Oid(KnownOid::ID_CA_ECDH_AES_CBC_CMAC_192) << ProtocolType::CA << KeyAgreementType::ECDH << MappingType::UNDEFINED << 24 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-CA-ECDH-AES-CBC-CMAC-256") << Oid(KnownOid::ID_CA_ECDH_AES_CBC_CMAC_256) << ProtocolType::CA << KeyAgreementType::ECDH << MappingType::UNDEFINED << 32 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;

			QTest::newRow("id-PACE-DH-GM-3DES-CBC-CBC") << Oid(KnownOid::ID_PACE_DH_GM_3DES_CBC_CBC) << ProtocolType::PACE << KeyAgreementType::DH << MappingType::GM << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("id-PACE-DH-GM-AES-CBC-CMAC-128") << Oid(KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_128) << ProtocolType::PACE << KeyAgreementType::DH << MappingType::GM << 16 << SignatureType::UNDEFINED << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-PACE-DH-GM-AES-CBC-CMAC-192") << Oid(KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_192) << ProtocolType::PACE << KeyAgreementType::DH << MappingType::GM << 24 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-PACE-DH-GM-AES-CBC-CMAC-256") << Oid(KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_256) << ProtocolType::PACE << KeyAgreementType::DH << MappingType::GM << 32 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;

			QTest::newRow("id-PACE-ECDH-GM-3DES-CBC-CBC") << Oid(KnownOid::ID_PACE_ECDH_GM_3DES_CBC_CBC) << ProtocolType::PACE << KeyAgreementType::ECDH << MappingType::GM << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("id-PACE-ECDH-GM-AES-CBC-CMAC-128") << Oid(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128) << ProtocolType::PACE << KeyAgreementType::ECDH << MappingType::GM << 16 << SignatureType::UNDEFINED << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-PACE-ECDH-GM-AES-CBC-CMAC-192") << Oid(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_192) << ProtocolType::PACE << KeyAgreementType::ECDH << MappingType::GM << 24 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-PACE-ECDH-GM-AES-CBC-CMAC-256") << Oid(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256) << ProtocolType::PACE << KeyAgreementType::ECDH << MappingType::GM << 32 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;

			QTest::newRow("id-PACE-DH-IM-3DES-CBC-CBC") << Oid(KnownOid::ID_PACE_DH_IM_3DES_CBC_CBC) << ProtocolType::PACE << KeyAgreementType::DH << MappingType::IM << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("id-PACE-DH-IM-AES-CBC-CMAC-128") << Oid(KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_128) << ProtocolType::PACE << KeyAgreementType::DH << MappingType::IM << 16 << SignatureType::UNDEFINED << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-PACE-DH-IM-AES-CBC-CMAC-192") << Oid(KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_192) << ProtocolType::PACE << KeyAgreementType::DH << MappingType::IM << 24 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-PACE-DH-IM-AES-CBC-CMAC-256") << Oid(KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_256) << ProtocolType::PACE << KeyAgreementType::DH << MappingType::IM << 32 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;

			QTest::newRow("id-PACE-ECDH-IM-3DES-CBC-CBC") << Oid(KnownOid::ID_PACE_ECDH_IM_3DES_CBC_CBC) << ProtocolType::PACE << KeyAgreementType::ECDH << MappingType::IM << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("id-PACE-ECDH-IM-AES-CBC-CMAC-128") << Oid(KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_128) << ProtocolType::PACE << KeyAgreementType::ECDH << MappingType::IM << 16 << SignatureType::UNDEFINED << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-PACE-ECDH-IM-AES-CBC-CMAC-192") << Oid(KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_192) << ProtocolType::PACE << KeyAgreementType::ECDH << MappingType::IM << 24 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-PACE-ECDH-IM-AES-CBC-CMAC-256") << Oid(KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_256) << ProtocolType::PACE << KeyAgreementType::ECDH << MappingType::IM << 32 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;

			QTest::newRow("id-RI-DH-SHA-1") << Oid(KnownOid::ID_RI_DH_SHA_1) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-RI-DH-SHA-224") << Oid(KnownOid::ID_RI_DH_SHA_224) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha224 << false;
			QTest::newRow("id-RI-DH-SHA-256") << Oid(KnownOid::ID_RI_DH_SHA_256) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-RI-DH-SHA-384") << Oid(KnownOid::ID_RI_DH_SHA_384) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha384 << false;
			QTest::newRow("id-RI-DH-SHA-512") << Oid(KnownOid::ID_RI_DH_SHA_512) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha512 << false;

			QTest::newRow("id-RI-ECDH-SHA-1") << Oid(KnownOid::ID_RI_ECDH_SHA_1) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha1 << false;
			QTest::newRow("id-RI-ECDH-SHA-224") << Oid(KnownOid::ID_RI_ECDH_SHA_224) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha224 << false;
			QTest::newRow("id-RI-ECDH-SHA-256") << Oid(KnownOid::ID_RI_ECDH_SHA_256) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << false;
			QTest::newRow("id-RI-ECDH-SHA-384") << Oid(KnownOid::ID_RI_ECDH_SHA_384) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha384 << false;
			QTest::newRow("id-RI-ECDH-SHA-512") << Oid(KnownOid::ID_RI_ECDH_SHA_512) << ProtocolType::RI << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha512 << false;

			QTest::newRow("unknown 1") << Oid() << ProtocolType::UNDEFINED << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("unknown 1") << Oid(QByteArray()) << ProtocolType::UNDEFINED << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("unknown 1") << Oid(QByteArray("unknown")) << ProtocolType::UNDEFINED << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("unknown 1") << Oid(QByteArray("0.4.0.127.0.7.2.2.1.1")) << ProtocolType::UNDEFINED << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("unknown 2") << Oid(QByteArray("0.4.0.127.0.7.2.2.1.1.1")) << ProtocolType::UNDEFINED << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
			QTest::newRow("unknown 3") << Oid(QByteArray("0.4.0.127.0.7.2.2.1.1.1.1")) << ProtocolType::UNDEFINED << KeyAgreementType::UNDEFINED << MappingType::UNDEFINED << 0 << SignatureType::UNDEFINED << QCryptographicHash::Sha256 << true;
		}


		void knownOIDs()
		{
			QFETCH(Oid, oid);
			QFETCH(ProtocolType, protocol);
			QFETCH(KeyAgreementType, keyAgreement);
			QFETCH(MappingType, mapping);
			QFETCH(int, keySize);
			QFETCH(SignatureType, signature);
			QFETCH(QCryptographicHash::Algorithm, hash);
			QFETCH(bool, causeWarnings);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			SecurityProtocol securityProtocol(oid);
			QCOMPARE(securityProtocol.getProtocol(), protocol);
			QCOMPARE(securityProtocol.getKeyAgreement(), keyAgreement);
			QCOMPARE(securityProtocol.getMapping(), mapping);
			QCOMPARE(securityProtocol.getKeySize(), keySize);
			QCOMPARE(securityProtocol.getSignature(), signature);
			QCOMPARE(securityProtocol.getHashAlgorithm(), hash);

			QCOMPARE(logSpy.count() > 0, causeWarnings);
		}


};

QTEST_GUILESS_MAIN(test_SecurityProtocol)
#include "test_SecurityProtocol.moc"
