/*!
 * \brief Unit tests for \ref RemoteDispatcher
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDispatcherClient.h"
#include "RemoteDispatcherServer.h"

#include "messages/IfdConnect.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdTransmit.h"
#include "MockDataChannel.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class RemoteDispatcherSpy
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<RemoteDispatcher> mRemoteDispatcher;
		bool mClosed;
		GlobalStatus::Code mCloseCode;
		QVector<RemoteCardMessageType> mReceivedMessageTypes;
		QVector<QJsonObject> mReceivedMessages;
		QVector<QString> mReceivedSignalSenders;

	public:
		RemoteDispatcherSpy(const QSharedPointer<RemoteDispatcher> pRemoteDispatcher);
		~RemoteDispatcherSpy() override;

		[[nodiscard]] bool isClosed() const;
		[[nodiscard]] GlobalStatus::Code getCloseCode() const;
		[[nodiscard]] const QVector<RemoteCardMessageType>& getReceivedMessageTypes() const;
		[[nodiscard]] const QVector<QJsonObject>& getReceivedMessages() const;
		[[nodiscard]] const QVector<QString>& getReceivedSignalSenders() const;

	private Q_SLOTS:
		void onClosed(GlobalStatus::Code pCloseCode, const QString& pId);
		void onReceived(RemoteCardMessageType pMessageType, const QJsonObject& pJsonObject, const QString& pId);
};


RemoteDispatcherSpy::RemoteDispatcherSpy(const QSharedPointer<RemoteDispatcher> pRemoteDispatcher)
	: mRemoteDispatcher(pRemoteDispatcher)
	, mClosed(false)
	, mCloseCode(GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose)
{
	const auto client = mRemoteDispatcher.objectCast<RemoteDispatcherClient>();
	if (client)
	{
		connect(client.data(), &RemoteDispatcherClient::fireClosed, this, &RemoteDispatcherSpy::onClosed);
		connect(client.data(), &RemoteDispatcherClient::fireReceived, this, &RemoteDispatcherSpy::onReceived);
		return;
	}

	const auto server = mRemoteDispatcher.objectCast<RemoteDispatcherServer>();
	if (server)
	{
		connect(server.data(), &RemoteDispatcherServer::fireClosed, this, &RemoteDispatcherSpy::onClosed);
		connect(server.data(), &RemoteDispatcherServer::fireReceived, this, &RemoteDispatcherSpy::onReceived);
		return;
	}
}


RemoteDispatcherSpy::~RemoteDispatcherSpy()
{
	const auto client = mRemoteDispatcher.objectCast<RemoteDispatcherClient>();
	if (client)
	{
		disconnect(client.data(), &RemoteDispatcherClient::fireClosed, this, &RemoteDispatcherSpy::onClosed);
		disconnect(client.data(), &RemoteDispatcherClient::fireReceived, this, &RemoteDispatcherSpy::onReceived);
		return;
	}

	const auto server = mRemoteDispatcher.objectCast<RemoteDispatcherServer>();
	if (server)
	{
		disconnect(server.data(), &RemoteDispatcherServer::fireClosed, this, &RemoteDispatcherSpy::onClosed);
		disconnect(server.data(), &RemoteDispatcherServer::fireReceived, this, &RemoteDispatcherSpy::onReceived);
		return;
	}
}


bool RemoteDispatcherSpy::isClosed() const
{
	return mClosed;
}


GlobalStatus::Code RemoteDispatcherSpy::getCloseCode() const
{
	return mCloseCode;
}


const QVector<RemoteCardMessageType>& RemoteDispatcherSpy::getReceivedMessageTypes() const
{
	return mReceivedMessageTypes;
}


const QVector<QJsonObject>& RemoteDispatcherSpy::getReceivedMessages() const
{
	return mReceivedMessages;
}


const QVector<QString>& RemoteDispatcherSpy::getReceivedSignalSenders() const
{
	return mReceivedSignalSenders;
}


void RemoteDispatcherSpy::onClosed(GlobalStatus::Code pCloseCode, const QString& pId)
{
	mClosed = true;
	mCloseCode = pCloseCode;
	mReceivedSignalSenders += pId;
}


void RemoteDispatcherSpy::onReceived(RemoteCardMessageType pMessageType, const QJsonObject& pJsonObject, const QString& pId)
{
	qDebug() << "RemoteDispatcherSpy::onReceived() -" << pMessageType;
	mReceivedMessageTypes += pMessageType;
	mReceivedMessages += pJsonObject;
	mReceivedSignalSenders += pId;
}


class test_RemoteDisp
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void channelClosedNormallyClient()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherClient> dispatcher(new RemoteDispatcherClient(IfdVersion::Version::v2, channel));
			RemoteDispatcherSpy spy(dispatcher);

			channel->close();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::No_Error);

			const QVector<QString>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first(), dispatcher->getId());
		}


		void channelClosedNormallyServer()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherServer> dispatcher(new RemoteDispatcherServer(channel));
			RemoteDispatcherSpy spy(dispatcher);

			channel->close();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::No_Error);

			const QVector<QString>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first(), dispatcher->getId());
		}


		void channelClosedAbnormallyClient()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherClient> dispatcher(new RemoteDispatcherClient(IfdVersion::Version::v2, channel));
			RemoteDispatcherSpy spy(dispatcher);

			channel->closeAbnormal();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);

			const QVector<QString>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first(), dispatcher->getId());
		}


		void channelClosedAbnormallyServer()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherServer> dispatcher(new RemoteDispatcherServer(channel));
			RemoteDispatcherSpy spy(dispatcher);

			channel->closeAbnormal();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);

			const QVector<QString>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first(), dispatcher->getId());
		}


		void messagesAreDelivered()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherClient> clientDispatcher(new RemoteDispatcherClient(IfdVersion::Version::v2, clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherServer> serverDispatcher(new RemoteDispatcherServer(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			RemoteDispatcherSpy spy(serverDispatcher);

			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdEstablishContext(IfdVersion::Version::v2, DeviceInfo::getName())));
			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdConnect(QStringLiteral("NFC Reader"))));
			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdTransmit(QStringLiteral("NFC Reader"), QByteArray::fromHex("00A402022F00"))));
			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdDisconnect(QStringLiteral("NFC Reader"))));

			const QVector<RemoteCardMessageType> receivedMessageTypes = spy.getReceivedMessageTypes();
			const QVector<QJsonObject> receivedMessages = spy.getReceivedMessages();
			QCOMPARE(receivedMessageTypes.size(), 3);
			QCOMPARE(receivedMessages.size(), 3);

			QCOMPARE(receivedMessageTypes.at(0), RemoteCardMessageType::IFDConnect);
			const IfdConnect ifdConnect(receivedMessages.at(0));
			QCOMPARE(ifdConnect.getType(), RemoteCardMessageType::IFDConnect);
			QCOMPARE(ifdConnect.getSlotName(), QStringLiteral("NFC Reader"));

			QCOMPARE(receivedMessageTypes.at(1), RemoteCardMessageType::IFDTransmit);
			const IfdTransmit ifdTransmit(receivedMessages.at(1));
			QCOMPARE(ifdTransmit.getType(), RemoteCardMessageType::IFDTransmit);
			QCOMPARE(ifdTransmit.getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(ifdTransmit.getInputApdu(), QByteArray::fromHex("00A402022F00"));

			QCOMPARE(receivedMessageTypes.at(2), RemoteCardMessageType::IFDDisconnect);
			const IfdDisconnect ifdDisconnect(receivedMessages.at(2));
			QCOMPARE(ifdDisconnect.getType(), RemoteCardMessageType::IFDDisconnect);
			QCOMPARE(ifdDisconnect.getSlotHandle(), QStringLiteral("NFC Reader"));

			const QVector<QString>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 3);
			QCOMPARE(senders.at(0), serverDispatcher->getId());
			QCOMPARE(senders.at(1), serverDispatcher->getId());
			QCOMPARE(senders.at(2), serverDispatcher->getId());
		}


		void channelIsClosedWhenRemoteDispatcherIsDestroyed()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			QSharedPointer<RemoteDispatcherClient> clientDispatcher(new RemoteDispatcherClient(IfdVersion::Version::v2, clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherServer> serverDispatcher(new RemoteDispatcherServer(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(clientChannel.data(), &DataChannel::fireClosed, serverChannel.data(), &DataChannel::close, Qt::DirectConnection);

			RemoteDispatcherSpy spy(serverDispatcher);
			QVERIFY(!spy.isClosed());
			// Destroying a remote dispatcher should close the underlying channel.
			clientDispatcher.reset();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::No_Error);

			const QVector<QString>& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.at(0), serverDispatcher->getId());
		}


		void repeatedIfdEstablishContextGenerateCorrectErrorMessage()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherClient> clientDispatcher(new RemoteDispatcherClient(IfdVersion::Version::v2, clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherServer> serverDispatcher(new RemoteDispatcherServer(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdEstablishContext(IfdVersion::Version::v2, DeviceInfo::getName())));
			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdEstablishContext(IfdVersion::Version::v2, DeviceInfo::getName())));

			const QVector<QByteArray>& clientReceivedDataBlocks = clientChannel->getReceivedDataBlocks();
			QCOMPARE(clientReceivedDataBlocks.size(), 2);

			const IfdEstablishContextResponse message1(RemoteMessage::parseByteArray(clientReceivedDataBlocks.at(0)));
			QVERIFY(!message1.isIncomplete());
			QCOMPARE(message1.getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(message1.resultHasError(), false);
			QCOMPARE(message1.getResultMinor(), ECardApiResult::Minor::null);

			const IfdEstablishContextResponse message2(RemoteMessage::parseByteArray(clientReceivedDataBlocks.at(1)));
			QVERIFY(!message2.isIncomplete());
			QCOMPARE(message2.getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(message2.resultHasError(), true);
			QCOMPARE(message2.getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);
		}


		void ifdEstablishContextWithWrongProtocolGeneratesCorrectErrorMessage()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherClient> clientDispatcher(new RemoteDispatcherClient(IfdVersion::Version::v2, clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcherServer> serverDispatcher(new RemoteDispatcherServer(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdEstablishContext(IfdVersion::Version::Unknown, DeviceInfo::getName())));

			const QVector<QByteArray>& clientReceivedDataBlocks = clientChannel->getReceivedDataBlocks();
			QCOMPARE(clientReceivedDataBlocks.size(), 1);

			const IfdEstablishContextResponse message(RemoteMessage::parseByteArray(clientReceivedDataBlocks.at(0)));
			QVERIFY(!message.isIncomplete());
			QCOMPARE(message.getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(message.resultHasError(), true);
			QCOMPARE(message.getResultMinor(), ECardApiResult::Minor::AL_Unknown_Error);
		}


};

QTEST_GUILESS_MAIN(test_RemoteDisp)
#include "test_RemoteDisp.moc"
