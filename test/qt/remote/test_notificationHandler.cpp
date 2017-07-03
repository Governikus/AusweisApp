/*!
 * \brief Unit tests for \ref NotificationParser
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */
#include "NotificationHandler.h"

#include "MockDataChannel.h"
#include "RemoteCardNotificationChecker.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;


class NotificationHandlerSpy
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<NotificationHandler> mNotificationHandler;
		bool mClosed;
		GlobalStatus::Code mCloseCode;
		QVector<QSharedPointer<const RemoteCardNotification> > mReceivedNotifications;

	public:
		NotificationHandlerSpy(const QSharedPointer<NotificationHandler> pNotificationHandler);
		virtual ~NotificationHandlerSpy();

		bool isClosed() const;
		GlobalStatus::Code getCloseCode() const;
		const QVector<QSharedPointer<const RemoteCardNotification> >& getReceivedNotifications() const;

	private Q_SLOTS:
		void onClosed(GlobalStatus::Code pCloseCode);
		void onReceived(const QSharedPointer<const RemoteCardNotification>& pNotification);
};


NotificationHandlerSpy::NotificationHandlerSpy(const QSharedPointer<NotificationHandler> pNotificationHandler)
	: mNotificationHandler(pNotificationHandler)
	, mClosed(false)
	, mCloseCode(GlobalStatus::Code::RemoteReader_CloseCode_Undefined)
{
	connect(mNotificationHandler.data(), &NotificationHandler::fireClosed, this, &NotificationHandlerSpy::onClosed);
	connect(mNotificationHandler.data(), &NotificationHandler::fireReceived, this, &NotificationHandlerSpy::onReceived);
}


NotificationHandlerSpy::~NotificationHandlerSpy()
{
	disconnect(mNotificationHandler.data(), &NotificationHandler::fireClosed, this, &NotificationHandlerSpy::onClosed);
	disconnect(mNotificationHandler.data(), &NotificationHandler::fireReceived, this, &NotificationHandlerSpy::onReceived);
}


bool NotificationHandlerSpy::isClosed() const
{
	return mClosed;
}


GlobalStatus::Code NotificationHandlerSpy::getCloseCode() const
{
	return mCloseCode;
}


const QVector<QSharedPointer<const RemoteCardNotification> >& NotificationHandlerSpy::getReceivedNotifications() const
{
	return mReceivedNotifications;
}


void NotificationHandlerSpy::onClosed(GlobalStatus::Code pCloseCode)
{
	mClosed = true;
	mCloseCode = pCloseCode;
}


void NotificationHandlerSpy::onReceived(const QSharedPointer<const RemoteCardNotification>& pNotification)
{
	mReceivedNotifications += pNotification;
}


class test_notificationHandler
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<RemoteCardNotificationChecker> mChecker;

	private Q_SLOTS:
		void channelClosedNormally()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<NotificationHandler> handler(new NotificationHandler(channel));
			NotificationHandlerSpy spy(handler);

			channel->close();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_NormalClose);
		}


		void channelClosedAbnormally()
		{
			const QSharedPointer<MockDataChannel> channel(new MockDataChannel());
			const QSharedPointer<NotificationHandler> handler(new NotificationHandler(channel));
			NotificationHandlerSpy spy(handler);

			channel->closeAbnormal();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);
		}


		void notificationsAreDelivered()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			const QSharedPointer<NotificationHandler> clientHandler(new NotificationHandler(clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<NotificationHandler> serverHandler(new NotificationHandler(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);

			NotificationHandlerSpy spy(serverHandler);

			clientHandler->send(QSharedPointer<const RemoteCardNotification>(new ConnectCmd(QStringLiteral("NFC Reader"))));
			clientHandler->send(QSharedPointer<const RemoteCardNotification>(new TransmitCmd(QStringLiteral("NFC Reader"), QByteArray::fromHex("00A402022F00"))));
			clientHandler->send(QSharedPointer<const RemoteCardNotification>(new DisconnectCmd(QStringLiteral("NFC Reader"))));

			const QVector<QSharedPointer<const RemoteCardNotification> > receivedNotifications = spy.getReceivedNotifications();
			QCOMPARE(receivedNotifications.size(), 3);
			QCOMPARE(receivedNotifications.at(0)->getType(), CardNotificationType::CONNECT);
			receivedNotifications.at(0)->accept(mChecker);

			QCOMPARE(receivedNotifications.at(1)->getType(), CardNotificationType::TRANSMIT);
			receivedNotifications.at(1)->accept(mChecker);

			QCOMPARE(receivedNotifications.at(2)->getType(), CardNotificationType::DISCONNECT);
			receivedNotifications.at(2)->accept(mChecker);
		}


		void channelIsClosedWhenNotificationHandlerIsDestroyed()
		{
			const QSharedPointer<MockDataChannel> clientChannel(new MockDataChannel());
			QSharedPointer<NotificationHandler> clientHandler(new NotificationHandler(clientChannel));

			const QSharedPointer<MockDataChannel> serverChannel(new MockDataChannel());
			const QSharedPointer<NotificationHandler> serverHandler(new NotificationHandler(serverChannel));

			connect(clientChannel.data(), &MockDataChannel::fireSend, serverChannel.data(), &MockDataChannel::onReceived, Qt::DirectConnection);
			connect(clientChannel.data(), &DataChannel::fireClosed, serverChannel.data(), &DataChannel::close, Qt::DirectConnection);

			NotificationHandlerSpy spy(serverHandler);
			QVERIFY(!spy.isClosed());
			// Destroying a notification handler should close the underlying channel.
			clientHandler.reset();
			QVERIFY(spy.isClosed());
			QCOMPARE(spy.getCloseCode(), GlobalStatus::Code::RemoteReader_CloseCode_NormalClose);
		}


	public:
		test_notificationHandler()
			: mChecker(new RemoteCardNotificationChecker())
		{
		}


};

QTEST_GUILESS_MAIN(test_notificationHandler)
#include "test_notificationHandler.moc"
