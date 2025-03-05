/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdDispatcherClient.h"
#include "IfdDispatcherServer.h"

#include "MockDataChannel.h"
#include "messages/IfdConnect.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdTransmit.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class IfdDispatcherSpy
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<IfdDispatcher> mDispatcher;
		bool mClosed;
		GlobalStatus::Code mCloseCode;
		QList<IfdMessageType> mReceivedMessageTypes;
		QList<QJsonObject> mReceivedMessages;
		QList<QByteArray> mReceivedSignalSenders;

	public:
		IfdDispatcherSpy(const QSharedPointer<IfdDispatcher> pDispatcher);
		~IfdDispatcherSpy() override;

		[[nodiscard]] bool isClosed() const;
		[[nodiscard]] GlobalStatus::Code getCloseCode() const;
		[[nodiscard]] const QList<IfdMessageType>& getReceivedMessageTypes() const;
		[[nodiscard]] const QList<QJsonObject>& getReceivedMessages() const;
		[[nodiscard]] const QList<QByteArray>& getReceivedSignalSenders() const;

	private Q_SLOTS:
		void onClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId);
		void onReceived(IfdMessageType pMessageType, const QJsonObject& pJsonObject, const QByteArray& pId);
};


IfdDispatcherSpy::IfdDispatcherSpy(const QSharedPointer<IfdDispatcher> pDispatcher)
	: mDispatcher(pDispatcher)
	, mClosed(false)
	, mCloseCode(GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose)
{
	const auto client = mDispatcher.objectCast<IfdDispatcherClient>();
	if (client)
	{
		connect(client.data(), &IfdDispatcherClient::fireClosed, this, &IfdDispatcherSpy::onClosed);
		connect(client.data(), &IfdDispatcherClient::fireReceived, this, &IfdDispatcherSpy::onReceived);
		return;
	}

	const auto server = mDispatcher.objectCast<IfdDispatcherServer>();
	if (server)
	{
		connect(server.data(), &IfdDispatcherServer::fireClosed, this, &IfdDispatcherSpy::onClosed);
		connect(server.data(), &IfdDispatcherServer::fireReceived, this, &IfdDispatcherSpy::onReceived);
		return;
	}
}


IfdDispatcherSpy::~IfdDispatcherSpy()
{
	const auto client = mDispatcher.objectCast<IfdDispatcherClient>();
	if (client)
	{
		disconnect(client.data(), &IfdDispatcherClient::fireClosed, this, &IfdDispatcherSpy::onClosed);
		disconnect(client.data(), &IfdDispatcherClient::fireReceived, this, &IfdDispatcherSpy::onReceived);
		return;
	}

	const auto server = mDispatcher.objectCast<IfdDispatcherServer>();
	if (server)
	{
		disconnect(server.data(), &IfdDispatcherServer::fireClosed, this, &IfdDispatcherSpy::onClosed);
		disconnect(server.data(), &IfdDispatcherServer::fireReceived, this, &IfdDispatcherSpy::onReceived);
		return;
	}
}


bool IfdDispatcherSpy::isClosed() const
{
	return mClosed;
}


GlobalStatus::Code IfdDispatcherSpy::getCloseCode() const
{
	return mCloseCode;
}


const QList<IfdMessageType>& IfdDispatcherSpy::getReceivedMessageTypes() const
{
	return mReceivedMessageTypes;
}


const QList<QJsonObject>& IfdDispatcherSpy::getReceivedMessages() const
{
	return mReceivedMessages;
}


const QList<QByteArray>& IfdDispatcherSpy::getReceivedSignalSenders() const
{
	return mReceivedSignalSenders;
}


void IfdDispatcherSpy::onClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId)
{
	mClosed = true;
	mCloseCode = pCloseCode;
	mReceivedSignalSenders += pId;
}


void IfdDispatcherSpy::onReceived(IfdMessageType pMessageType, const QJsonObject& pJsonObject, const QByteArray& pId)
{
	qDebug() << "IfdDispatcherSpy::onReceived() -" << pMessageType;
	mReceivedMessageTypes += pMessageType;
	mReceivedMessages += pJsonObject;
	mReceivedSignalSenders += pId;
}


class test_IfdDispatcher
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void channelClosedNormallyClient()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherClient> dispatcher(new IfdDispatcherClient(IfdVersion::Version::v2, channel));
			IfdDispatcherSpy spy(dispatcher);

			channel->close();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::No_Error);

			const QList<QByteArray>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first(), dispatcher->getId());
		}


		void channelClosedNormallyServer()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherServer> dispatcher(new IfdDispatcherServer(channel));
			IfdDispatcherSpy spy(dispatcher);

			channel->close();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::No_Error);

			const QList<QByteArray>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first(), dispatcher->getId());
		}


		void channelClosedAbnormallyClient()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherClient> dispatcher(new IfdDispatcherClient(IfdVersion::Version::v2, channel));
			IfdDispatcherSpy spy(dispatcher);

			channel->closeAbnormal();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);

			const QList<QByteArray>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first(), dispatcher->getId());
		}


		void channelClosedAbnormallyServer()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherServer> dispatcher(new IfdDispatcherServer(channel));
			IfdDispatcherSpy spy(dispatcher);

			channel->closeAbnormal();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);

			const QList<QByteArray>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first(), dispatcher->getId());
		}


		void messagesAreDelivered()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherClient> clientDispatcher(new IfdDispatcherClient(IfdVersion::Version::v2, clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherServer> serverDispatcher(new IfdDispatcherServer(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			IfdDispatcherSpy spy(serverDispatcher);

			clientDispatcher->send(QSharedPointer<const IfdMessage>(new IfdEstablishContext(IfdVersion::Version::v2, DeviceInfo::getName())));
			clientDispatcher->send(QSharedPointer<const IfdMessage>(new IfdConnect(QStringLiteral("NFC Reader"))));
			clientDispatcher->send(QSharedPointer<const IfdMessage>(new IfdTransmit(QStringLiteral("NFC Reader"), QByteArray::fromHex("00A402022F00"))));
			clientDispatcher->send(QSharedPointer<const IfdMessage>(new IfdDisconnect(QStringLiteral("NFC Reader"))));

			const QList<IfdMessageType> receivedMessageTypes = spy.getReceivedMessageTypes();
			const QList<QJsonObject> receivedMessages = spy.getReceivedMessages();
			QCOMPARE(receivedMessageTypes.size(), 3);
			QCOMPARE(receivedMessages.size(), 3);

			QCOMPARE(receivedMessageTypes.at(0), IfdMessageType::IFDConnect);
			const IfdConnect ifdConnect(receivedMessages.at(0));
			QCOMPARE(ifdConnect.getType(), IfdMessageType::IFDConnect);
			QCOMPARE(ifdConnect.getSlotName(), QStringLiteral("NFC Reader"));

			QCOMPARE(receivedMessageTypes.at(1), IfdMessageType::IFDTransmit);
			const IfdTransmit ifdTransmit(receivedMessages.at(1));
			QCOMPARE(ifdTransmit.getType(), IfdMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray::fromHex("00A402022F00"));

			QCOMPARE(receivedMessageTypes.at(2), IfdMessageType::IFDDisconnect);
			const IfdDisconnect ifdDisconnect(receivedMessages.at(2));
			QCOMPARE(ifdDisconnect.getType(), IfdMessageType::IFDDisconnect);
			QCOMPARE(ifdDisconnect.getSlotHandle(), QStringLiteral("NFC Reader"));

			const QList<QByteArray>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 3);
			QCOMPARE(senders.at(0), serverDispatcher->getId());
			QCOMPARE(senders.at(1), serverDispatcher->getId());
			QCOMPARE(senders.at(2), serverDispatcher->getId());
		}


		void channelIsClosedWhenDispatcherIsDestroyed()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			QSharedPointer<IfdDispatcherClient> clientDispatcher(new IfdDispatcherClient(IfdVersion::Version::v2, clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherServer> serverDispatcher(new IfdDispatcherServer(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(clientChannel.data(), &DataChannel::fireClosed, serverChannel.data(), &DataChannel::close, Qt::DirectConnection);

			IfdDispatcherSpy spy(serverDispatcher);
			QVERIFY(!spy.isClosed());
			// Destroying a remote dispatcher should close the underlying channel.
			clientDispatcher.reset();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::No_Error);

			const QList<QByteArray>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.at(0), serverDispatcher->getId());
		}


		void repeatedIfdEstablishContextGenerateCorrectErrorMessage()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherClient> clientDispatcher(new IfdDispatcherClient(IfdVersion::Version::v2, clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherServer> serverDispatcher(new IfdDispatcherServer(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			clientDispatcher->send(QSharedPointer<const IfdMessage>(new IfdEstablishContext(IfdVersion::Version::v2, DeviceInfo::getName())));
			clientDispatcher->send(QSharedPointer<const IfdMessage>(new IfdEstablishContext(IfdVersion::Version::v2, DeviceInfo::getName())));

			const QList<QByteArray>& clientReceivedDataBlocks = clientChannel->getReceivedDataBlocks();
			QCOMPARE(clientReceivedDataBlocks.size(), 2);

			const IfdEstablishContextResponse message1(IfdMessage::parseByteArray(clientReceivedDataBlocks.at(0)));
			QVERIFY(!message1.isIncomplete());
			QCOMPARE(message1.getType(), IfdMessageType::IFDEstablishContextResponse);
			QCOMPARE(message1.resultHasError(), false);
			QCOMPARE(message1.getResultMinor(), ECardApiResult::Minor::null);

			const IfdEstablishContextResponse message2(IfdMessage::parseByteArray(clientReceivedDataBlocks.at(1)));
			QVERIFY(!message2.isIncomplete());
			QCOMPARE(message2.getType(), IfdMessageType::IFDEstablishContextResponse);
			QCOMPARE(message2.resultHasError(), true);
			QCOMPARE(message2.getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);
		}


		void ifdEstablishContextWithWrongProtocolGeneratesCorrectErrorMessage()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherClient> clientDispatcher(new IfdDispatcherClient(IfdVersion::Version::v2, clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<IfdDispatcherServer> serverDispatcher(new IfdDispatcherServer(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			clientDispatcher->send(QSharedPointer<const IfdMessage>(new IfdEstablishContext(IfdVersion::Version::Unknown, DeviceInfo::getName())));

			const QList<QByteArray>& clientReceivedDataBlocks = clientChannel->getReceivedDataBlocks();
			QCOMPARE(clientReceivedDataBlocks.size(), 1);

			const IfdEstablishContextResponse message(IfdMessage::parseByteArray(clientReceivedDataBlocks.at(0)));
			QVERIFY(!message.isIncomplete());
			QCOMPARE(message.getType(), IfdMessageType::IFDEstablishContextResponse);
			QCOMPARE(message.resultHasError(), true);
			QCOMPARE(message.getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);
		}


		void isNormalConnection()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel(false));
			const QSharedPointer<IfdDispatcherClient> clientDispatcher(new IfdDispatcherClient(IfdVersion::Version::v2, clientChannel));
			QVERIFY(!clientDispatcher->isPairingConnection());
		}


		void isPairingConnection()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel(true));
			const QSharedPointer<IfdDispatcherClient> clientDispatcher(new IfdDispatcherClient(IfdVersion::Version::v2, clientChannel));
			QVERIFY(clientDispatcher->isPairingConnection());
		}


};

QTEST_GUILESS_MAIN(test_IfdDispatcher)
#include "test_IfdDispatcher.moc"
