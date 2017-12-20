/*!
 * \brief Unit tests for \ref RemoteDispatcher
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDispatcherImpl.h"

#include "messages/IfdConnect.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdTransmit.h"
#include "MockDataChannel.h"
#include "RemoteMessageChecker.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>


using namespace governikus;


class RemoteDispatcherSpy
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<RemoteDispatcher> mRemoteDispatcher;
		bool mClosed;
		GlobalStatus::Code mCloseCode;
		QVector<QSharedPointer<const RemoteMessage> > mReceivedMessages;
		QVector<QSharedPointer<RemoteDispatcher> > mReceivedSignalSenders;

	public:
		RemoteDispatcherSpy(const QSharedPointer<RemoteDispatcher> pRemoteDispatcher);
		virtual ~RemoteDispatcherSpy();

		bool isClosed() const;
		GlobalStatus::Code getCloseCode() const;
		const QVector<QSharedPointer<const RemoteMessage> >& getReceivedMessages() const;
		const QVector<QSharedPointer<RemoteDispatcher> >& getReceivedSignalSenders() const;

	private Q_SLOTS:
		void onClosed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pDispatcher);
		void onReceived(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pDispatcher);
};


RemoteDispatcherSpy::RemoteDispatcherSpy(const QSharedPointer<RemoteDispatcher> pRemoteDispatcher)
	: mRemoteDispatcher(pRemoteDispatcher)
	, mClosed(false)
	, mCloseCode(GlobalStatus::Code::RemoteReader_CloseCode_Undefined)
{
	connect(mRemoteDispatcher.data(), &RemoteDispatcher::fireClosed, this, &RemoteDispatcherSpy::onClosed);
	connect(mRemoteDispatcher.data(), &RemoteDispatcher::fireReceived, this, &RemoteDispatcherSpy::onReceived);
}


RemoteDispatcherSpy::~RemoteDispatcherSpy()
{
	disconnect(mRemoteDispatcher.data(), &RemoteDispatcher::fireClosed, this, &RemoteDispatcherSpy::onClosed);
	disconnect(mRemoteDispatcher.data(), &RemoteDispatcher::fireReceived, this, &RemoteDispatcherSpy::onReceived);
}


bool RemoteDispatcherSpy::isClosed() const
{
	return mClosed;
}


GlobalStatus::Code RemoteDispatcherSpy::getCloseCode() const
{
	return mCloseCode;
}


const QVector<QSharedPointer<const RemoteMessage> >& RemoteDispatcherSpy::getReceivedMessages() const
{
	return mReceivedMessages;
}


const QVector<QSharedPointer<RemoteDispatcher> >& RemoteDispatcherSpy::getReceivedSignalSenders() const
{
	return mReceivedSignalSenders;
}


void RemoteDispatcherSpy::onClosed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pDispatcher)
{
	mClosed = true;
	mCloseCode = pCloseCode;
	mReceivedSignalSenders += pDispatcher;
}


void RemoteDispatcherSpy::onReceived(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pDispatcher)
{
	qDebug() << "RemoteDispatcherSpy::onReceived() -" << pMessage->getType();
	mReceivedMessages += pMessage;
	mReceivedSignalSenders += pDispatcher;
}


class test_RemoteDisp
	: public QObject
{
	Q_OBJECT

	private:
		RemoteMessageChecker mChecker;

	private Q_SLOTS:
		void channelClosedNormally()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> dispatcher(new RemoteDispatcherImpl(channel));
			RemoteDispatcherSpy spy(dispatcher);

			channel->close();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_NormalClose);

			const QVector<QSharedPointer<RemoteDispatcher> >& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first().data(), dispatcher.data());
		}


		void channelClosedAbnormally()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> dispatcher(new RemoteDispatcherImpl(channel));
			RemoteDispatcherSpy spy(dispatcher);

			channel->closeAbnormal();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);

			const QVector<QSharedPointer<RemoteDispatcher> >& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.first().data(), dispatcher.data());
		}


		void messagesAreDelivered()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> clientDispatcher(new RemoteDispatcherImpl(clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> serverDispatcher(new RemoteDispatcherImpl(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			RemoteDispatcherSpy spy(serverDispatcher);

			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdEstablishContext(QStringLiteral("IFDInterface_WebSocket_v0"), DeviceInfo::getName())));
			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdConnect(QStringLiteral("NFC Reader"))));
			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdTransmit(QStringLiteral("NFC Reader"), QByteArray::fromHex("00A402022F00"))));
			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdDisconnect(QStringLiteral("NFC Reader"))));

			const QVector<QSharedPointer<const RemoteMessage> > receivedMessages = spy.getReceivedMessages();
			QCOMPARE(receivedMessages.size(), 3);
			QCOMPARE(receivedMessages.at(0)->getType(), RemoteCardMessageType::IFDConnect);
			mChecker.receive(receivedMessages.at(0));

			QCOMPARE(receivedMessages.at(1)->getType(), RemoteCardMessageType::IFDTransmit);
			mChecker.receive(receivedMessages.at(1));

			QCOMPARE(receivedMessages.at(2)->getType(), RemoteCardMessageType::IFDDisconnect);
			mChecker.receive(receivedMessages.at(2));

			const QVector<QSharedPointer<RemoteDispatcher> >& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 3);
			QCOMPARE(senders.at(0).data(), serverDispatcher.data());
			QCOMPARE(senders.at(1).data(), serverDispatcher.data());
			QCOMPARE(senders.at(2).data(), serverDispatcher.data());
		}


		void channelIsClosedWhenRemoteDispatcherIsDestroyed()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			QSharedPointer<RemoteDispatcher> clientDispatcher(new RemoteDispatcherImpl(clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> serverDispatcher(new RemoteDispatcherImpl(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(clientChannel.data(), &DataChannel::fireClosed, serverChannel.data(), &DataChannel::close, Qt::DirectConnection);

			RemoteDispatcherSpy spy(serverDispatcher);
			QVERIFY(!spy.isClosed());
			// Destroying a remote dispatcher should close the underlying channel.
			clientDispatcher.reset();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_NormalClose);

			const QVector<QSharedPointer<RemoteDispatcher> >& senders = spy.getReceivedSignalSenders();
			QCOMPARE(senders.size(), 1);
			QCOMPARE(senders.at(0).data(), serverDispatcher.data());
		}


		void repeatedIfdEstablishContextGenerateCorrectErrorMessage()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> clientDispatcher(new RemoteDispatcherImpl(clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> serverDispatcher(new RemoteDispatcherImpl(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdEstablishContext(QStringLiteral("IFDInterface_WebSocket_v0"), DeviceInfo::getName())));
			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdEstablishContext(QStringLiteral("IFDInterface_WebSocket_v0"), DeviceInfo::getName())));

			const QVector<QByteArray>& clientReceivedDataBlocks = clientChannel->getReceivedDataBlocks();
			QCOMPARE(clientReceivedDataBlocks.size(), 2);
			const QSharedPointer<const IfdEstablishContextResponse> message1 = RemoteMessageParser().parse(clientReceivedDataBlocks.at(0)).dynamicCast<const IfdEstablishContextResponse>();
			QVERIFY(!message1.isNull());
			QCOMPARE(message1->getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(message1->resultHasError(), false);
			QCOMPARE(message1->getResultMinor(), QString());

			const QSharedPointer<const IfdEstablishContextResponse> message2 = RemoteMessageParser().parse(clientReceivedDataBlocks.at(1)).dynamicCast<const IfdEstablishContextResponse>();
			QVERIFY(!message2.isNull());
			QCOMPARE(message2->getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(message2->resultHasError(), true);
			QCOMPARE(message2->getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError"));
		}


		void ifdEstablishContextWithWrongProtocolGeneratesCorrectErrorMessage()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> clientDispatcher(new RemoteDispatcherImpl(clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<RemoteDispatcher> serverDispatcher(new RemoteDispatcherImpl(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(serverChannel.data(), &MockDataChannel::fireSend, clientChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			clientDispatcher->send(QSharedPointer<const RemoteMessage>(new IfdEstablishContext(QStringLiteral("IFDInterface_WebSocket_v2"), DeviceInfo::getName())));

			const QVector<QByteArray>& clientReceivedDataBlocks = clientChannel->getReceivedDataBlocks();
			QCOMPARE(clientReceivedDataBlocks.size(), 1);
			const QSharedPointer<const RemoteMessage> message = RemoteMessageParser().parse(clientReceivedDataBlocks.at(0));
			QVERIFY(!message.isNull());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			const QSharedPointer<const IfdEstablishContextResponse> ifdEstablishContextResponse = message.dynamicCast<const IfdEstablishContextResponse>();
			QVERIFY(!ifdEstablishContextResponse.isNull());
			QCOMPARE(ifdEstablishContextResponse->resultHasError(), true);
			QCOMPARE(ifdEstablishContextResponse->getResultMinor(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError"));
		}


	public:
		test_RemoteDisp()
			: mChecker()
		{
		}


};

QTEST_GUILESS_MAIN(test_RemoteDisp)
#include "test_RemoteDispImpl.moc"
