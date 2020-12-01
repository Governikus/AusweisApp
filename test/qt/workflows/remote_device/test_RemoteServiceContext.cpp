/*!
 * \brief Unit tests for the class \ref RemoteServiceContext.
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "context/RemoteServiceContext.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


class test_RemoteServiceContext
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_NewPin()
		{
			RemoteServiceContext context;
			const QString pin1 = QStringLiteral("567890");
			const QString pin2 = QStringLiteral("654321");

			context.setNewPin(pin1);
			QCOMPARE(context.getNewPin(), pin1);

			context.setNewPin(pin2);
			QCOMPARE(context.getNewPin(), pin2);
		}


		void test_ModifyPinMessage()
		{
			RemoteServiceContext context;
			const QSharedPointer<const IfdModifyPin> message(new IfdModifyPin(QString("message"), QByteArray("message")));
			const QSharedPointer<const IfdModifyPin> emptyMessage(new IfdModifyPin(QString(), QByteArray()));

			context.setModifyPinMessage(message);
			QCOMPARE(context.getModifyPinMessage(), message);

			context.setModifyPinMessage(emptyMessage);
			QCOMPARE(context.getModifyPinMessage(), emptyMessage);
		}


		void test_OnResetMessageHandler()
		{
			RemoteServiceContext context;
			context.onResetMessageHandler();
			QCOMPARE(context.getCardConnection(), QSharedPointer<CardConnection>());
			QCOMPARE(context.getCan(), QString());
			QCOMPARE(context.getPin(), QString());
			QCOMPARE(context.getPuk(), QString());
			QCOMPARE(context.getNewPin(), QString());
			QCOMPARE(context.getLastPaceResult(), CardReturnCode::OK);
			QVERIFY(!context.hasEstablishedPaceChannelRequest());
			QCOMPARE(context.getModifyPinMessage(), QSharedPointer<const IfdModifyPin>());
		}


		void test_EstablishPaceChannelMessage()
		{
			RemoteServiceContext context;
			const QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(QString("message"), QByteArray("message")));

			context.setEstablishPaceChannelMessage(message);
			QVERIFY(context.hasEstablishedPaceChannelRequest());

			context.setEstablishPaceChannelMessage(nullptr);
			QVERIFY(!context.hasEstablishedPaceChannelRequest());
		}


};

QTEST_GUILESS_MAIN(test_RemoteServiceContext)
#include "test_RemoteServiceContext.moc"
