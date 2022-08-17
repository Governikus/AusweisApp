/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
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
		void setAuxiliaryData()
		{
			CommandData data;
			QByteArray auxData = QByteArray::fromHex("670F0102030405060708090A0B0C0D0E0F");
			data.append(auxData);
			QCOMPARE(data, auxData);
			QCOMPARE(data.get(CommandData::AUXILIARY_AUTHENTICATED_DATA), auxData.mid(2));
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


		void setPrivateKey()
		{
			CommandData data;
			data.append(CommandData::PRIVATE_KEY_REFERENCE, 2);
			QCOMPARE(data, QByteArray::fromHex("840102"));
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


		void setChat()
		{
			CommandData data;
			QByteArray chat = QByteArray::fromHex("7f4C05A9D9E9F949");
			data.append(chat);
			QCOMPARE(data, chat);
			QCOMPARE(data.get(CommandData::CERTIFICATE_HOLDER_AUTHORIZATION_TEMPLATE), chat.mid(3));
		}


		void setCertificateSignature()
		{
			CommandData data;
			QByteArray signature = QByteArray::fromHex("5f37050102030405");
			data.append(signature);
			QCOMPARE(data, signature);
			QCOMPARE(data.get(CommandData::CERTIFICATE_SIGNATURE), signature.mid(3));
		}


		void setCertificateBody()
		{
			CommandData data;
			QByteArray body = QByteArray::fromHex("7f4e050102030405");
			data.append(body);
			QCOMPARE(data, body);
			QCOMPARE(data.get(CommandData::CERTIFICATE_BODY), body.mid(3));
		}


};

QTEST_GUILESS_MAIN(test_CommandData)
#include "test_CommandData.moc"
