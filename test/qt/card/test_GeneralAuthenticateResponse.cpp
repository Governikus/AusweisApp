/*!
 * \brief Tests for GeneralAuthenticate response APDUs
 *
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "GeneralAuthenticateResponse.h"


using namespace governikus;


class test_GeneralAuthenticateResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseGAEncryptedNonceResponse()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c1280105391ded7867c2d7df7f871ed6913c07d9000"));

			const GAEncryptedNonceResponse response(apdu);

			QCOMPARE(response.getEncryptedNonce(), QByteArray::fromHex("5391ded7867c2d7df7f871ed6913c07d"));
		}


		void parseGAEncryptedNonceResponse_invalidData()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c1281105391ded7867c2d7df7f871ed6913c07d9000"));

			const GAEncryptedNonceResponse response(apdu);

			QCOMPARE(response.getEncryptedNonce(), QByteArray());
		}


		void parseGAMapNonceResponse()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c438241042a8199d469fde8f98e22bf8bb5a72804b5293bb54a8afa4d84e4b63217d163b61d78dc6453408bde19a86254ee3b0f03871964b71f1b57f77037ecdbedbe79b09000"));

			const GAMapNonceResponse response(apdu);

			QCOMPARE(response.getMappingData(), QByteArray::fromHex("042a8199d469fde8f98e22bf8bb5a72804b5293bb54a8afa4d84e4b63217d163b61d78dc6453	408bde19a86254ee3b0f03871964b71f1b57f77037ecdbedbe79b0"));
		}


		void parseGAMapNonceResponse_invalidData()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c438141042a8199d469fde8f98e22bf8bb5a72804b5293bb54a8afa4d84e4b63217d163b61d78dc6453408bde19a86254ee3b0f03871964b71f1b57f77037ecdbedbe79b09000"));

			const GAMapNonceResponse response(apdu);

			QCOMPARE(response.getMappingData(), QByteArray());
		}


		void parseGAPerformKeyAgreementResponse()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c43844104a3be2ed0fccb4bf96df00be39a9c3e6b67d3a1118c95c195d0389fa14956c383a322c34f1b63a7bdb41f98b644aa9e15f823a2d726ef6ae8df3c10ac4e7298cc9000"));

			const GAPerformKeyAgreementResponse response(apdu);

			QCOMPARE(response.getEphemeralPublicKey(), QByteArray::fromHex("04a3be2ed0fccb4bf96df00be39a9c3e6b67d3a1118c95c195d0389fa14956c383a322c34f1b63a7bdb41f98b644aa9e15f823a2d726ef6ae8df3c10ac4e7298cc"));
		}


		void parseGAPerformKeyAgreementResponse_invalid()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c43814104a3be2ed0fccb4bf96df00be39a9c3e6b67d3a1118c95c195d0389fa14956c383a322c34f1b63a7bdb41f98b644aa9e15f823a2d726ef6ae8df3c10ac4e7298cc9000"));

			const GAPerformKeyAgreementResponse response(apdu);

			QCOMPARE(response.getEphemeralPublicKey(), QByteArray());
		}


		void parseGAMutualAuthenticationResponse_withoutCARs()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c0a8608afcd013365384ba39000"));

			const GAMutualAuthenticationResponse response(apdu);

			QCOMPARE(response.getAuthenticationToken(), QByteArray::fromHex("afcd013365384ba3"));
			QCOMPARE(response.getCarCurr(), QByteArray());
			QCOMPARE(response.getCarPrev(), QByteArray());
		}


		void testGAMutualAuthenticationResponse_oneCAR()
		{
			// this is the response from a new card that did not see any link certificates
			const ResponseApdu apdu(QByteArray::fromHex("7c1a860871204ff538eec464870e44454356434165494430303130339000"));

			const GAMutualAuthenticationResponse response(apdu);

			QCOMPARE(response.getCarCurr(), QByteArray("DECVCAeID00103"));
			QCOMPARE(response.getCarPrev(), QByteArray());
		}


		void testGAMutualAuthenticationResponse_twoCARs()
		{
			// this is the response from a card that already saw link certificates
			const ResponseApdu apdu(QByteArray::fromHex("7c2a86086fa6266f2ef1f2d9870e4445544553546549443030303034880e44455445535465494430303030329000"));

			const GAMutualAuthenticationResponse response(apdu);

			QCOMPARE(response.getCarCurr(), QByteArray("DETESTeID00004"));
			QCOMPARE(response.getCarPrev(), QByteArray("DETESTeID00002"));
		}


		void testGAMutualAuthenticationResponse_invalid()
		{
			// this is the response from a new card that did not see any link certificates
			const ResponseApdu apdu(QByteArray::fromHex("7c1a810871204ff538eec464870e44454356434165494430303130339000"));

			const GAMutualAuthenticationResponse response(apdu);

			QCOMPARE(response.getCarCurr(), QByteArray());
			QCOMPARE(response.getCarPrev(), QByteArray());
		}


		void testGAChipAuthenticationResponse()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c1481085b5b32c5b15d012c8208aaa14cfba15994d39000"));

			const GAChipAuthenticationResponse response(apdu);

			QCOMPARE(response.getNonce(), QByteArray::fromHex("5b5b32c5b15d012c"));
			QCOMPARE(response.getAuthenticationToken(), QByteArray::fromHex("aaa14cfba15994d3"));
		}


		void testGAChipAuthenticationResponse_invalid()
		{
			const ResponseApdu apdu(QByteArray::fromHex("7c148208aaa14cfba15994d39000"));

			const GAChipAuthenticationResponse response(apdu);

			QCOMPARE(response.getAuthenticationToken(), QByteArray());
			QCOMPARE(response.getNonce(), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_GeneralAuthenticateResponse)
#include "test_GeneralAuthenticateResponse.moc"
