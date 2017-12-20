/*!
 * \brief Unit tests for \ref KnownOIDs
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/KnownOIDs.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_KnownOIDs
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void values_data()
		{
			QTest::addColumn<QByteArray>("id");
			QTest::addColumn<QByteArray>("expected");

			QTest::newRow("base_bsi_de") << toByteArray(KnownOIDs::Base::BSI_DE) << QByteArray("0.4.0.127.0.7");
			QTest::newRow("base_signed_data") << toByteArray(KnownOIDs::Base::SIGNED_DATA) << QByteArray("1.2.840.113549.1.7.2");
			QTest::newRow("base_id_security_object") << toByteArray(KnownOIDs::Base::ID_SECURITY_OBJECT) << QByteArray("0.4.0.127.0.7.3.2.1");
			QTest::newRow("base_id_extensions") << toByteArray(KnownOIDs::Base::ID_EXTENSIONS) << QByteArray("0.4.0.127.0.7.3.1.3");

			QTest::newRow("id_description") << toByteArray(KnownOIDs::CertificateExtensions::ID_DESCRIPTION) << QByteArray("0.4.0.127.0.7.3.1.3.1");
			QTest::newRow("id_sector") << toByteArray(KnownOIDs::CertificateExtensions::ID_SECTOR) << QByteArray("0.4.0.127.0.7.3.1.3.2");

			QTest::newRow("id_plainFormat") << toByteArray(KnownOIDs::TermsOfUsageType::ID_PLAIN_FORMAT) << QByteArray("0.4.0.127.0.7.3.1.3.1.1");
			QTest::newRow("id_htmlFormat") << toByteArray(KnownOIDs::TermsOfUsageType::ID_HTML_FORMAT) << QByteArray("0.4.0.127.0.7.3.1.3.1.2");
			QTest::newRow("id_pdfFormat") << toByteArray(KnownOIDs::TermsOfUsageType::ID_PDF_FORMAT) << QByteArray("0.4.0.127.0.7.3.1.3.1.3");

			QTest::newRow("id_IS") << toByteArray(KnownOIDs::CHATType::ID_IS) << QByteArray("0.4.0.127.0.7.3.1.2.1");
			QTest::newRow("id_AT") << toByteArray(KnownOIDs::CHATType::ID_AT) << QByteArray("0.4.0.127.0.7.3.1.2.2");
			QTest::newRow("id_ST") << toByteArray(KnownOIDs::CHATType::ID_ST) << QByteArray("0.4.0.127.0.7.3.1.2.3");

			QTest::newRow("id_DateOfBirth") << toByteArray(KnownOIDs::AuxilaryData::ID_DATE_OF_BIRTH) << QByteArray("0.4.0.127.0.7.3.1.4.1");
			QTest::newRow("id_DateOfExpiry") << toByteArray(KnownOIDs::AuxilaryData::ID_DATE_OF_EXPIRY) << QByteArray("0.4.0.127.0.7.3.1.4.2");
			QTest::newRow("id_CommunityID") << toByteArray(KnownOIDs::AuxilaryData::ID_COMMUNITY_ID) << QByteArray("0.4.0.127.0.7.3.1.4.3");

			QTest::newRow("SecurityProtocol::id_PK") << toByteArray(KnownOIDs::SecurityProtocol::ID_PK) << QByteArray("0.4.0.127.0.7.2.2.1");
			QTest::newRow("SecurityProtocol::id_TA") << toByteArray(KnownOIDs::SecurityProtocol::ID_TA) << QByteArray("0.4.0.127.0.7.2.2.2");
			QTest::newRow("SecurityProtocol::id_CA") << toByteArray(KnownOIDs::SecurityProtocol::ID_CA) << QByteArray("0.4.0.127.0.7.2.2.3");
			QTest::newRow("SecurityProtocol::id_PACE") << toByteArray(KnownOIDs::SecurityProtocol::ID_PACE) << QByteArray("0.4.0.127.0.7.2.2.4");

			QTest::newRow("id_CA::DH") << toByteArray(KnownOIDs::id_ca::DH) << QByteArray("0.4.0.127.0.7.2.2.3.1");
			QTest::newRow("id_CA::DH_3DES_CBC_CBC") << toByteArray(KnownOIDs::id_ca::DH_3DES_CBC_CBC) << QByteArray("0.4.0.127.0.7.2.2.3.1.1");
			QTest::newRow("id_CA::DH_AES_CBC_CMAC_128") << toByteArray(KnownOIDs::id_ca::DH_AES_CBC_CMAC_128) << QByteArray("0.4.0.127.0.7.2.2.3.1.2");
			QTest::newRow("id_CA::DH_AES_CBC_CMAC_192") << toByteArray(KnownOIDs::id_ca::DH_AES_CBC_CMAC_192) << QByteArray("0.4.0.127.0.7.2.2.3.1.3");
			QTest::newRow("id_CA::DH_AES_CBC_CMAC_256") << toByteArray(KnownOIDs::id_ca::DH_AES_CBC_CMAC_256) << QByteArray("0.4.0.127.0.7.2.2.3.1.4");
			QTest::newRow("id_CA::ECDH") << toByteArray(KnownOIDs::id_ca::ECDH) << QByteArray("0.4.0.127.0.7.2.2.3.2");
			QTest::newRow("id_CA::ECDH_3DES_CBC_CBC") << toByteArray(KnownOIDs::id_ca::ECDH_3DES_CBC_CBC) << QByteArray("0.4.0.127.0.7.2.2.3.2.1");
			QTest::newRow("id_CA::ECDH_AES_CBC_CMAC_128") << toByteArray(KnownOIDs::id_ca::ECDH_AES_CBC_CMAC_128) << QByteArray("0.4.0.127.0.7.2.2.3.2.2");
			QTest::newRow("id_CA::ECDH_AES_CBC_CMAC_192") << toByteArray(KnownOIDs::id_ca::ECDH_AES_CBC_CMAC_192) << QByteArray("0.4.0.127.0.7.2.2.3.2.3");
			QTest::newRow("id_CA::ECDH_AES_CBC_CMAC_256") << toByteArray(KnownOIDs::id_ca::ECDH_AES_CBC_CMAC_256) << QByteArray("0.4.0.127.0.7.2.2.3.2.4");

			QTest::newRow("id_TA::ECDSA_SHA_1") << toByteArray(KnownOIDs::id_ta::ECDSA_SHA_1) << QByteArray("0.4.0.127.0.7.2.2.2.2.1");
			QTest::newRow("id_TA::ECDSA_SHA_224") << toByteArray(KnownOIDs::id_ta::ECDSA_SHA_224) << QByteArray("0.4.0.127.0.7.2.2.2.2.2");
			QTest::newRow("id_TA::ECDSA_SHA_256") << toByteArray(KnownOIDs::id_ta::ECDSA_SHA_256) << QByteArray("0.4.0.127.0.7.2.2.2.2.3");
			QTest::newRow("id_TA::ECDSA_SHA_384") << toByteArray(KnownOIDs::id_ta::ECDSA_SHA_384) << QByteArray("0.4.0.127.0.7.2.2.2.2.4");
			QTest::newRow("id_TA::ECDSA_SHA_512") << toByteArray(KnownOIDs::id_ta::ECDSA_SHA_512) << QByteArray("0.4.0.127.0.7.2.2.2.2.5");

			QTest::newRow("id_PK::DH") << toByteArray(KnownOIDs::id_pk::DH) << QByteArray("0.4.0.127.0.7.2.2.1.1");
			QTest::newRow("id_PK::ECDH") << toByteArray(KnownOIDs::id_pk::ECDH) << QByteArray("0.4.0.127.0.7.2.2.1.2");

			QTest::newRow("id_PACE::DH::GM") << toByteArray(KnownOIDs::id_PACE::DH::GM) << QByteArray("0.4.0.127.0.7.2.2.4.1");
			QTest::newRow("id_PACE::DH::GM_3DES_CBC_CBC") << toByteArray(KnownOIDs::id_PACE::DH::GM_3DES_CBC_CBC) << QByteArray("0.4.0.127.0.7.2.2.4.1.1");
			QTest::newRow("id_PACE::DH::GM_AES_CBC_CMAC_128") << toByteArray(KnownOIDs::id_PACE::DH::GM_AES_CBC_CMAC_128) << QByteArray("0.4.0.127.0.7.2.2.4.1.2");
			QTest::newRow("id_PACE::DH::GM_AES_CBC_CMAC_192") << toByteArray(KnownOIDs::id_PACE::DH::GM_AES_CBC_CMAC_192) << QByteArray("0.4.0.127.0.7.2.2.4.1.3");
			QTest::newRow("id_PACE::DH::GM_AES_CBC_CMAC_256") << toByteArray(KnownOIDs::id_PACE::DH::GM_AES_CBC_CMAC_256) << QByteArray("0.4.0.127.0.7.2.2.4.1.4");
			QTest::newRow("id_PACE::DH::IM") << toByteArray(KnownOIDs::id_PACE::DH::IM) << QByteArray("0.4.0.127.0.7.2.2.4.3");
			QTest::newRow("id_PACE::DH::IM_3DES_CBC_CBC") << toByteArray(KnownOIDs::id_PACE::DH::IM_3DES_CBC_CBC) << QByteArray("0.4.0.127.0.7.2.2.4.3.1");
			QTest::newRow("id_PACE::DH::IM_AES_CBC_CMAC_128") << toByteArray(KnownOIDs::id_PACE::DH::IM_AES_CBC_CMAC_128) << QByteArray("0.4.0.127.0.7.2.2.4.3.2");
			QTest::newRow("id_PACE::DH::IM_AES_CBC_CMAC_192") << toByteArray(KnownOIDs::id_PACE::DH::IM_AES_CBC_CMAC_192) << QByteArray("0.4.0.127.0.7.2.2.4.3.3");
			QTest::newRow("id_PACE::DH::IM_AES_CBC_CMAC_256") << toByteArray(KnownOIDs::id_PACE::DH::IM_AES_CBC_CMAC_256) << QByteArray("0.4.0.127.0.7.2.2.4.3.4");

			QTest::newRow("id_PACE::ECDH::GM") << toByteArray(KnownOIDs::id_PACE::ECDH::GM) << QByteArray("0.4.0.127.0.7.2.2.4.2");
			QTest::newRow("id_PACE::ECDH::GM_3DES_CBC_CBC") << toByteArray(KnownOIDs::id_PACE::ECDH::GM_3DES_CBC_CBC) << QByteArray("0.4.0.127.0.7.2.2.4.2.1");
			QTest::newRow("id_PACE::ECDH::GM_AES_CBC_CMAC_128") << toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128) << QByteArray("0.4.0.127.0.7.2.2.4.2.2");
			QTest::newRow("id_PACE::ECDH::GM_AES_CBC_CMAC_192") << toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_192) << QByteArray("0.4.0.127.0.7.2.2.4.2.3");
			QTest::newRow("id_PACE::ECDH::GM_AES_CBC_CMAC_256") << toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256) << QByteArray("0.4.0.127.0.7.2.2.4.2.4");
			QTest::newRow("id_PACE::ECDH::IM") << toByteArray(KnownOIDs::id_PACE::ECDH::IM) << QByteArray("0.4.0.127.0.7.2.2.4.4");
			QTest::newRow("id_PACE::ECDH::IM_3DES_CBC_CBC") << toByteArray(KnownOIDs::id_PACE::ECDH::IM_3DES_CBC_CBC) << QByteArray("0.4.0.127.0.7.2.2.4.4.1");
			QTest::newRow("id_PACE::ECDH::IM_AES_CBC_CMAC_128") << toByteArray(KnownOIDs::id_PACE::ECDH::IM_AES_CBC_CMAC_128) << QByteArray("0.4.0.127.0.7.2.2.4.4.2");
			QTest::newRow("id_PACE::ECDH::IM_AES_CBC_CMAC_192") << toByteArray(KnownOIDs::id_PACE::ECDH::IM_AES_CBC_CMAC_192) << QByteArray("0.4.0.127.0.7.2.2.4.4.3");
			QTest::newRow("id_PACE::ECDH::IM_AES_CBC_CMAC_256") << toByteArray(KnownOIDs::id_PACE::ECDH::IM_AES_CBC_CMAC_256) << QByteArray("0.4.0.127.0.7.2.2.4.4.4");
		}


		void values()
		{
			QFETCH(QByteArray, id);
			QFETCH(QByteArray, expected);
			QCOMPARE(id, expected);
		}


};

QTEST_GUILESS_MAIN(test_KnownOIDs)
#include "test_KnownOIDs.moc"
