/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Tests for GeneralAuthenticate response APDUs
 */

#include <QtCore>
#include <QtTest>

#include "apdu/GeneralAuthenticateResponse.h"


using namespace governikus;


class test_GeneralAuthenticateResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseGAEncryptedNonceResponse()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C1280105391DED7867C2D7DF7F871ED6913C07D9000"));

			const GAEncryptedNonceResponse response(apdu);

			QCOMPARE(response.getEncryptedNonce(), QByteArray::fromHex("5391DED7867C2d7DF7F871ED6913C07D"));
		}


		void parseGAEncryptedNonceResponse_invalidData()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C1281105391DED7867C2D7DF7F871ED6913C07D9000"));

			const GAEncryptedNonceResponse response(apdu);

			QCOMPARE(response.getEncryptedNonce(), QByteArray());
		}


		void parseGAMapNonceResponse()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C438241042A8199D469FDE8F98E22BF8BB5A72804B5293BB54A8AFA4D84E4B63217D163B61D78DC6453408BDE19A86254EE3B0F03871964B71F1B57F77037ECDBEDBE79B09000"));

			const GAMapNonceResponse response(apdu);

			QCOMPARE(response.getMappingData(), QByteArray::fromHex("042A8199D469FDE8F98E22BF8BB5A72804B5293BB54A8AFA4D84E4B63217D163B61D78DC6453	408BDE19A86254EE3B0F03871964B71F1B57F77037ECDBEDBE79B0"));
		}


		void parseGAMapNonceResponse_invalidData()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C438141042A8199D469FDE8F98E22BF8BB5A72804B5293BB54A8AFA4D84E4B63217D163B61D78DC6453408BDE19A86254EE3B0F03871964B71F1B57F77037ECDBEDBE79B09000"));

			const GAMapNonceResponse response(apdu);

			QCOMPARE(response.getMappingData(), QByteArray());
		}


		void parseGAPerformKeyAgreementResponse()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C43844104A3BE2ED0FCCB4BF96DF00BE39A9C3E6B67D3A1118C95C195D0389FA14956C383A322C34F1B63A7BDB41F98B644AA9E15F823A2D726EF6AE8DF3C10AC4E7298CC9000"));

			const GAPerformKeyAgreementResponse response(apdu);

			QCOMPARE(response.getEphemeralPublicKey(), QByteArray::fromHex("04A3BE2ED0FCCB4BF96DF00BE39A9C3E6B67D3A1118C95C195D0389FA14956C383A322C34F1B63A7BDB41F98B644AA9E15F823A2D726EF6AE8DF3C10AC4E7298CC"));
		}


		void parseGAPerformKeyAgreementResponse_invalid()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C43814104A3BE2ED0FCCB4BF96DF00BE39A9C3E6B67D3A1118C95C195D0389FA14956C383A322C34F1B63A7BDB41F98B644AA9E15F823A2D726EF6AE8DF3C10AC4E7298CC9000"));

			const GAPerformKeyAgreementResponse response(apdu);

			QCOMPARE(response.getEphemeralPublicKey(), QByteArray());
		}


		void parseGAMutualAuthenticationResponse_withoutCARs()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C0A8608AFCD013365384BA39000"));

			const GAMutualAuthenticationResponse response(apdu);

			QCOMPARE(response.getAuthenticationToken(), QByteArray::fromHex("AFCD013365384BA3"));
			QCOMPARE(response.getCarCurr(), QByteArray());
			QCOMPARE(response.getCarPrev(), QByteArray());
		}


		void testGAMutualAuthenticationResponse_oneCAR()
		{
			// this is the response from a new card that did not see any link certificates
			const ResponseApdu apdu(QByteArray::fromHex("7C1A860871204FF538EEC464870E44454356434165494430303130339000"));

			const GAMutualAuthenticationResponse response(apdu);

			QCOMPARE(response.getCarCurr(), QByteArray("DECVCAeID00103"));
			QCOMPARE(response.getCarPrev(), QByteArray());
		}


		void testGAMutualAuthenticationResponse_twoCARs()
		{
			// this is the response from a card that already saw link certificates
			const ResponseApdu apdu(QByteArray::fromHex("7C2A86086FA6266F2EF1F2D9870E4445544553546549443030303034880E44455445535465494430303030329000"));

			const GAMutualAuthenticationResponse response(apdu);

			QCOMPARE(response.getCarCurr(), QByteArray("DETESTeID00004"));
			QCOMPARE(response.getCarPrev(), QByteArray("DETESTeID00002"));
		}


		void testGAMutualAuthenticationResponse_invalid()
		{
			// this is the response from a new card that did not see any link certificates
			const ResponseApdu apdu(QByteArray::fromHex("7C1A810871204FF538EEC464870E44454356434165494430303130339000"));

			const GAMutualAuthenticationResponse response(apdu);

			QCOMPARE(response.getCarCurr(), QByteArray());
			QCOMPARE(response.getCarPrev(), QByteArray());
		}


		void testGAChipAuthenticationResponse()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C1481085B5B32C5B15D012C8208AAA14CFBA15994D39000"));

			const GAChipAuthenticationResponse response(apdu);

			QCOMPARE(response.getNonce(), QByteArray::fromHex("5B5B32C5B15D012C"));
			QCOMPARE(response.getAuthenticationToken(), QByteArray::fromHex("AAA14CFBA15994D3"));
		}


		void testGAChipAuthenticationResponse_invalid()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7C148208AAA14CFBA15994D39000"));

			const GAChipAuthenticationResponse response(apdu);

			QCOMPARE(response.getAuthenticationToken(), QByteArray());
			QCOMPARE(response.getNonce(), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_GeneralAuthenticateResponse)
#include "test_GeneralAuthenticateResponse.moc"
