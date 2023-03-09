/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "apdu/CommandData.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_CommandData
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void encodeDecode_data()
		{
			QTest::addColumn<int>("objectCount");
			QTest::addColumn<QByteArray>("data");

			// S = Simple, C = Complex
			QTest::newRow("Empty") << 1 << QByteArray();
			QTest::newRow("S") << 1 << QByteArray::fromHex("81 01 FF");
			QTest::newRow("S,S") << 3 << QByteArray::fromHex("82 01 FF"
															 "83 01 FE");
			QTest::newRow("C(S)") << 2 << QByteArray::fromHex("A1 03"
															  "   82 01 FF");
			QTest::newRow("S,C(S)") << 4 << QByteArray::fromHex("82 01 FF"
																"A3 03"
																"   84 01 FE");
			QTest::newRow("C(S),S") << 4 << QByteArray::fromHex("A2 03"
																"   83 01 FF"
																"84 01 FE");
			QTest::newRow("C(S),C(S)") << 5 << QByteArray::fromHex("A2 03"
																   "   83 01 FF"
																   "A4 03"
																   "   85 01 FE");
			QTest::newRow("C(S,S)") << 3 << QByteArray::fromHex("A1 06"
																"   82 01 FF"
																"   83 01 FE");
			QTest::newRow("C(C(S))") << 3 << QByteArray::fromHex("A1 05"
																 "   A2 03"
																 "      83 01 FF");
			QTest::newRow("C(S,C(S))") << 4 << QByteArray::fromHex("A1 08"
																   "   82 01 FF"
																   "   A3 03"
																   "      84 01 FE");
			QTest::newRow("C(C(S),S)") << 4 << QByteArray::fromHex("A1 08"
																   "   A2 03"
																   "      83 01 FF"
																   "   84 01 FE");
			QTest::newRow("C(C(S),C(S))") << 5 << QByteArray::fromHex("A1 0A"
																	  "   A2 03"
																	  "      83 01 FF"
																	  "   A4 03"
																	  "      85 01 FE");
		}


		void encodeDecode()
		{
			QFETCH(int, objectCount);
			QFETCH(QByteArray, data);

			const CommandData cmd(data);
			QCOMPARE(cmd.getObjectCount(), objectCount);
			QCOMPARE(QByteArray(cmd), data);
		}


		void encodeInteger_data()
		{
			QTest::addColumn<int>("number");
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("-1") << -1 << QByteArray::fromHex("8401FF");
			QTest::newRow("0") << 0 << QByteArray::fromHex("840100");
			QTest::newRow("1") << 1 << QByteArray::fromHex("840101");
			QTest::newRow("127") << 127 << QByteArray::fromHex("84017F");
			QTest::newRow("128") << 128 << QByteArray::fromHex("84020080");
			QTest::newRow("256") << 256 << QByteArray::fromHex("84020100");
			QTest::newRow("32767") << 32767 << QByteArray::fromHex("84027FFF");
			QTest::newRow("32768") << 32768 << QByteArray::fromHex("8403008000");
		}


		void encodeInteger()
		{
			QFETCH(int, number);
			QFETCH(QByteArray, data);

			CommandData cmd;
			cmd.append(CommandData::PRIVATE_KEY_REFERENCE, number);
			QCOMPARE(cmd, data);
		}


		void setOid()
		{
			CommandData data;
			Oid oid(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128);
			data.append(CommandData::CRYPTOGRAPHIC_MECHANISM_REFERENCE, oid);
			QCOMPARE(data, QByteArray::fromHex("800A").append(oid.getData()));
		}


		void setPublicKey()
		{
			CommandData data;
			QByteArray publicKey = QByteArray::fromHex("A9D9E9F9493954939495");
			data.append(CommandData::PACE_EPHEMERAL_PUBLIC_KEY, publicKey);
			QCOMPARE(data, QByteArray::fromHex("830A").append(publicKey));
		}


		void setPublicKeyId()
		{
			CommandData data;
			PacePasswordId pinId = PacePasswordId::PACE_PIN;
			data.append(CommandData::PUBLIC_KEY_REFERENCE, pinId);
			QCOMPARE(data, QByteArray::fromHex("8301").append(static_cast<char>(pinId)));
		}


		void setPaceMappingData()
		{
			CommandData data;
			QByteArray mapping = QByteArray::fromHex("A9D9E9F949395493999994");
			data.append(CommandData::MAPPING_DATA, mapping);
			QCOMPARE(data, QByteArray::fromHex("810B").append(mapping));
		}


		void setPaceAuthenticationToken()
		{
			CommandData data;
			QByteArray token = QByteArray::fromHex("A9D9E9F949395493999994");
			data.append(CommandData::AUTHENTICATION_TOKEN, token);
			QCOMPARE(data, QByteArray::fromHex("850B").append(token));
		}


		void setPaceEphemeralPublicKey()
		{
			CommandData data;
			QByteArray publicKey = QByteArray::fromHex("A9D9E9F949395493999994");
			data.append(CommandData::PACE_EPHEMERAL_PUBLIC_KEY, publicKey);
			QCOMPARE(data, QByteArray::fromHex("830B").append(publicKey));
		}


		void setCaEphemeralPublicKey()
		{
			CommandData data;
			QByteArray publicKey = QByteArray::fromHex("A9D9E9F949395493999994");
			data.append(CommandData::CA_EPHEMERAL_PUBLIC_KEY, publicKey);
			QCOMPARE(data, QByteArray::fromHex("800B").append(publicKey));
		}


		void setTaEphemeralPublicKey()
		{
			CommandData data;
			QByteArray publicKey = QByteArray::fromHex("A9D9E9F949395493999994");
			data.append(CommandData::TA_EPHEMERAL_PUBLIC_KEY, publicKey);
			QCOMPARE(data, QByteArray::fromHex("910B").append(publicKey));
		}


		void getDataObject_data()
		{
			QTest::addColumn<QByteArray>("asn1");
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<QByteArray>("object");

			// S = Simple, C = Complex
			QTest::newRow("Empty") << QByteArray() << QByteArray() << QByteArray();
			QTest::newRow("S MISS") << QByteArray::fromHex("82 01 FF")
									<< QByteArray()
									<< QByteArray();
			QTest::newRow("S HIT") << QByteArray::fromHex("81 01 FF")
								   << QByteArray::fromHex("FF")
								   << QByteArray::fromHex("81 01 FF");
			QTest::newRow("S,S MISS") << QByteArray::fromHex("82 01 FF"
															 "83 01 FE")
									  << QByteArray()
									  << QByteArray();
			QTest::newRow("S,S HIT 1") << QByteArray::fromHex("81 01 FF"
															  "82 01 FE")
									   << QByteArray::fromHex("FF")
									   << QByteArray::fromHex("81 01 FF");
			QTest::newRow("S,S HIT 2") << QByteArray::fromHex("82 01 FE"
															  "81 01 FF")
									   << QByteArray::fromHex("FF")
									   << QByteArray::fromHex("81 01 FF");
			QTest::newRow("S,S HIT 3") << QByteArray::fromHex("81 01 FF"
															  "81 01 FE")
									   << QByteArray::fromHex("FF")
									   << QByteArray::fromHex("81 01 FF");
			QTest::newRow("C(S) MISS") << QByteArray::fromHex("A2 03"
															  "   82 01 FF")
									   << QByteArray()
									   << QByteArray();
			QTest::newRow("C(S) HIT 1") << QByteArray::fromHex("A1 03"
															   "   81 01 FF")
										<< QByteArray::fromHex("FF")
										<< QByteArray::fromHex("A1 03 81 01 FF");
			QTest::newRow("C(S) HIT 2") << QByteArray::fromHex("A2 03"
															   "   81 01 FF")
										<< QByteArray::fromHex("FF")
										<< QByteArray::fromHex("81 01 FF");
			QTest::newRow("C(S) MISS/HIT") << QByteArray::fromHex("A1 03"
																  "   82 01 FF")
										   << QByteArray()
										   << QByteArray::fromHex("A1 03 82 01 FF");
		}


		void getDataObject()
		{
			QFETCH(QByteArray, asn1);
			QFETCH(QByteArray, data);
			QFETCH(QByteArray, object);

			const CommandData cmd(asn1);
			QCOMPARE(cmd.getData(V_ASN1_CONTEXT_SPECIFIC, CommandData::MAPPING_DATA), data);
			QCOMPARE(cmd.getObject(V_ASN1_CONTEXT_SPECIFIC, CommandData::MAPPING_DATA), object);
		}


		void setCertificateSignature()
		{
			CommandData data;
			QByteArray signature = QByteArray::fromHex("5f37050102030405");
			data.append(signature);
			QCOMPARE(data, signature);
			QCOMPARE(data.getData(V_ASN1_APPLICATION, CommandData::CERTIFICATE_SIGNATURE), signature.mid(3));
			QCOMPARE(data.getObject(V_ASN1_APPLICATION, CommandData::CERTIFICATE_SIGNATURE), signature);
		}


		void setAuxiliaryData()
		{
			CommandData data;
			QByteArray auxData = QByteArray::fromHex("6703800101");
			data.append(auxData);
			QCOMPARE(data, auxData);
			QCOMPARE(data.getData(V_ASN1_APPLICATION, CommandData::AUXILIARY_AUTHENTICATED_DATA), QByteArray());
			QCOMPARE(data.getObject(V_ASN1_APPLICATION, CommandData::AUXILIARY_AUTHENTICATED_DATA), auxData);
		}


		void setChat()
		{
			CommandData data;
			QByteArray chat = QByteArray::fromHex("7f4C03800101");
			data.append(chat);
			QCOMPARE(data, chat);
			QCOMPARE(data.getData(V_ASN1_APPLICATION, CommandData::CERTIFICATE_HOLDER_AUTHORIZATION_TEMPLATE), QByteArray());
			QCOMPARE(data.getObject(V_ASN1_APPLICATION, CommandData::CERTIFICATE_HOLDER_AUTHORIZATION_TEMPLATE), chat);
		}


		void setCertificateBody()
		{
			CommandData data;
			QByteArray body = QByteArray::fromHex("7f4e03800101");
			data.append(body);
			QCOMPARE(data, body);
			QCOMPARE(data.getData(V_ASN1_APPLICATION, CommandData::CERTIFICATE_BODY), QByteArray());
			QCOMPARE(data.getObject(V_ASN1_APPLICATION, CommandData::CERTIFICATE_BODY), body);
		}


};

QTEST_GUILESS_MAIN(test_CommandData)
#include "test_CommandData.moc"
