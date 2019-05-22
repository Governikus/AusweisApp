/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Commands.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;

class test_MSEBuilder
	: public QObject
{
	Q_OBJECT

	QSharedPointer<MSEBuilder> mMseBuilder;

	void assertCommandContent(const QByteArray& pExpectedContent)
	{
		QCOMPARE(mMseBuilder->build().getData(), pExpectedContent);
	}


	private Q_SLOTS:
		void init()
		{
			mMseBuilder.reset(new MSEBuilder(MSEBuilder::P1::PERFORM_SECURITY_OPERATION, MSEBuilder::P2::SET_AT));
		}


		void setAuxiliaryData()
		{
			QByteArray data = QByteArray::fromHex("0102030405060708090A0B0C0D0E0F");
			mMseBuilder->setAuxiliaryData(data);
			assertCommandContent(data);
		}


		void setOid()
		{
			QByteArray data = QByteArray::fromHex("1929394959697989796959493929");
			mMseBuilder->setOid(data);
			assertCommandContent(QByteArray::fromHex("800E").append(data));
		}


		void setPublicKey()
		{
			QByteArray data = QByteArray::fromHex("a9d9e9f9493954939495");
			mMseBuilder->setPublicKey(data);
			assertCommandContent(QByteArray::fromHex("830A").append(data));
		}


		void setPublicKeyId()
		{
			PacePasswordId pinId = PacePasswordId::PACE_PIN;
			mMseBuilder->setPublicKey(pinId);
			assertCommandContent(QByteArray::fromHex("8301").append(static_cast<char>(pinId)));
		}


		void setPrivateKey()
		{
			QByteArray data = QByteArray::fromHex("a9d9e9f94939549394");
			mMseBuilder->setPrivateKey(data);
			assertCommandContent(QByteArray::fromHex("8409").append(data));
		}


		void setEphemeralPublicKey()
		{
			QByteArray data = QByteArray::fromHex("a9d9e9f949395493999994");
			mMseBuilder->setEphemeralPublicKey(data);
			assertCommandContent(QByteArray::fromHex("910B").append(data));
		}


		void setChat()
		{
			QByteArray data = QByteArray::fromHex("a9d9e9f949");
			mMseBuilder->setChat(data);
			assertCommandContent(data);
		}


};

QTEST_GUILESS_MAIN(test_MSEBuilder)
#include "test_MSEBuilder.moc"
