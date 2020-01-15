/*!
 * \brief Tests for the class Commands.
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "EABuilder.h"
#include "GABuilder.h"
#include "PSOBuilder.h"

#include <QtTest>

using namespace governikus;

class test_Commands
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_SetCertificateBody_PSOBuilder()
		{
			PSOBuilder builder(PSOBuilder::P1::VERIFY, PSOBuilder::P2::CERTIFICATE);
			const QByteArray data("data");

			builder.setCertificateBody(data);
			QCOMPARE(builder.mCertificateBody, data);
		}


		void test_SetSignature_PSOBuilder()
		{
			PSOBuilder builder(PSOBuilder::P1::VERIFY, PSOBuilder::P2::CERTIFICATE);
			const QByteArray data("data");

			builder.setSignature(data);
			QCOMPARE(builder.mSignature, data);
		}


		void test_Build_PSOBuilder()
		{
			PSOBuilder builder(PSOBuilder::P1::VERIFY, PSOBuilder::P2::CERTIFICATE);

			CommandApdu apdu = builder.build();
			QCOMPARE(apdu.getINS(), 0x2a);
			QCOMPARE(apdu.getP1(), 0x00);
			QCOMPARE(apdu.getP2(), '\xbe');
			QCOMPARE(apdu.getData(), QByteArray());
		}


		void test_SetSignatureEABuilder()
		{
			EABuilder builder;
			const QByteArray data("data");

			builder.setSignature(data);
			QCOMPARE(builder.mSignature, data);
		}


		void test_Build_EABuilder()
		{
			EABuilder builder;
			const QByteArray data("data");

			builder.setSignature(data);
			CommandApdu apdu = builder.build();

			QCOMPARE(apdu.getINS(), char(0x82));
			QCOMPARE(apdu.getP1(), 0);
			QCOMPARE(apdu.getP2(), 0);
			QCOMPARE(apdu.getData(), data);
		}


		void test_SetCaEphemeralPublicKey_GABuilder()
		{
			GABuilder builder;
			const QByteArray data("");

			builder.setCaEphemeralPublicKey(data);
			QCOMPARE(builder.mCaEphemeralPublicKey, QByteArray::fromHex("8000"));
		}


		void test_SetPaceMappingData_GABuilder()
		{
			GABuilder builder;
			const QByteArray data("");

			builder.setPaceMappingData(data);
			QCOMPARE(builder.mPaceMappingData, QByteArray::fromHex("8100"));
		}


		void test_SetPaceEphemeralPublicKey_GABuilder()
		{
			GABuilder builder;
			const QByteArray data("");

			builder.setPaceEphemeralPublicKey(data);
			QCOMPARE(builder.mPaceEphemeralPublicKey, QByteArray::fromHex("8300"));
		}


		void test_SetPaceAuthenticationToken_GABuilder()
		{
			GABuilder builder;
			const QByteArray data("");

			builder.setPaceAuthenticationToken(data);
			QCOMPARE(builder.mPaceAuthenticationToken, QByteArray::fromHex("8500"));
		}


};

QTEST_GUILESS_MAIN(test_Commands)
#include "test_Commands.moc"
