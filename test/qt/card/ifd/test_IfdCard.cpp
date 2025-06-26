/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdCard.h"

#include "MockIfdDispatcher.h"

#include <QtTest>


using namespace governikus;


class test_IfdCard
	: public QObject
{
	Q_OBJECT

	private:
		QThread mNetworkThread;
		QSharedPointer<MockIfdDispatcher> mDispatcher;

	private Q_SLOTS:
		void init()
		{
			mNetworkThread.setObjectName(QStringLiteral("NetworkThread"));
			mNetworkThread.start();

			mDispatcher.reset(new MockIfdDispatcher());
			mDispatcher->moveToThread(&mNetworkThread);
		}


		void cleanup()
		{
			mDispatcher.reset();

			mNetworkThread.quit();
			mNetworkThread.wait();
		}


		void timeout()
		{
			IfdCard card(mDispatcher, QStringLiteral("IfdReader"));
			qputenv("SAC_RESPONSE_TIMEOUT", "100");

			QTest::ignoreMessage(QtDebugMsg, "Expected answer ( IFDConnectResponse ) was not received within 100 ms.");
			QCOMPARE(card.establishConnection(), CardReturnCode::INPUT_TIME_OUT);

			QTest::ignoreMessage(QtDebugMsg, "Expected answer ( IFDDisconnectResponse ) was not received within 100 ms.");
			QCOMPARE(card.releaseConnection(), CardReturnCode::INPUT_TIME_OUT);

			QTest::ignoreMessage(QtDebugMsg, "Expected answer ( IFDTransmitResponse ) was not received within 100 ms.");
			QCOMPARE(card.transmit(CommandApdu()).mReturnCode, CardReturnCode::INPUT_TIME_OUT);

			QTest::ignoreMessage(QtDebugMsg, "Expected answer ( IFDEstablishPACEChannelResponse ) was not received within 100 ms.");
			QCOMPARE(card.establishPaceChannel(PacePasswordId::PACE_PIN, 6, QByteArray(), QByteArray()), EstablishPaceChannelOutput(CardReturnCode::INPUT_TIME_OUT));

			QTest::ignoreMessage(QtDebugMsg, "Expected answer ( IFDDestroyPACEChannelResponse ) was not received within 100 ms.");
			QCOMPARE(card.destroyPaceChannel(), CardReturnCode::INPUT_TIME_OUT);

			QTest::ignoreMessage(QtDebugMsg, "Expected answer ( IFDModifyPINResponse ) was not received within 100 ms.");
			QCOMPARE(card.setEidPin(1).mReturnCode, CardReturnCode::INPUT_TIME_OUT);

			qunsetenv("SAC_RESPONSE_TIMEOUT");
		}


		void invalid()
		{
			mDispatcher->setState(MockIfdDispatcher::DispatcherState::ReaderInvalid);

			IfdCard card(mDispatcher, QStringLiteral("IfdReader"));

			QTest::ignoreMessage(QtWarningMsg, "Invalid messageType received: \"IFDTest\"");
			QTest::ignoreMessage(QtCriticalMsg, "The value of msg should be IFDConnectResponse");
			QCOMPARE(card.establishConnection(), CardReturnCode::COMMAND_FAILED);

			QTest::ignoreMessage(QtWarningMsg, "Invalid messageType received: \"IFDTest\"");
			QTest::ignoreMessage(QtCriticalMsg, "The value of msg should be IFDDisconnectResponse");
			QCOMPARE(card.releaseConnection(), CardReturnCode::COMMAND_FAILED);

			QTest::ignoreMessage(QtWarningMsg, "Invalid messageType received: \"IFDTest\"");
			QTest::ignoreMessage(QtCriticalMsg, "The value of msg should be IFDTransmitResponse");
			QCOMPARE(card.transmit(CommandApdu()).mReturnCode, CardReturnCode::COMMAND_FAILED);

			QTest::ignoreMessage(QtWarningMsg, "Invalid messageType received: \"IFDTest\"");
			QTest::ignoreMessage(QtCriticalMsg, "The value of msg should be IFDEstablishPACEChannelResponse");
			QCOMPARE(card.establishPaceChannel(PacePasswordId::PACE_PIN, 6, QByteArray(), QByteArray()), EstablishPaceChannelOutput(CardReturnCode::COMMAND_FAILED));

			QTest::ignoreMessage(QtWarningMsg, "Invalid messageType received: \"IFDTest\"");
			QTest::ignoreMessage(QtCriticalMsg, "The value of msg should be IFDDestroyPACEChannelResponse");
			QCOMPARE(card.destroyPaceChannel(), CardReturnCode::COMMAND_FAILED);

			QTest::ignoreMessage(QtWarningMsg, "Invalid messageType received: \"IFDTest\"");
			QTest::ignoreMessage(QtCriticalMsg, "The value of msg should be IFDModifyPINResponse");
			QCOMPARE(card.setEidPin(1).mReturnCode, CardReturnCode::COMMAND_FAILED);
		}


		void respectRemoveCard()
		{
			IfdCard card(mDispatcher, QStringLiteral("IfdReader"));

			mDispatcher->setState(MockIfdDispatcher::DispatcherState::ReaderWithCard);
			QCOMPARE(card.establishPaceChannel(PacePasswordId::PACE_PIN, 6, QByteArray(), QByteArray()), EstablishPaceChannelOutput(CardReturnCode::OK));
			QCOMPARE(card.setEidPin(1).mReturnCode, CardReturnCode::OK);

			mDispatcher->setState(MockIfdDispatcher::DispatcherState::ReaderWithCardRemoved);
			QTest::ignoreMessage(QtDebugMsg, "Card was removed while waiting for IFDEstablishPACEChannelResponse");
			QTest::ignoreMessage(QtWarningMsg, "Ignoring unexpected message type: IFDStatus");
			QCOMPARE(card.establishPaceChannel(PacePasswordId::PACE_PIN, 6, QByteArray(), QByteArray()), EstablishPaceChannelOutput(CardReturnCode::CARD_NOT_FOUND));
			QTest::ignoreMessage(QtDebugMsg, "Card was removed while waiting for IFDModifyPINResponse");
			QTest::ignoreMessage(QtWarningMsg, "Ignoring unexpected message type: IFDStatus");
			QCOMPARE(card.setEidPin(1).mReturnCode, CardReturnCode::CARD_NOT_FOUND);
		}


};

QTEST_GUILESS_MAIN(test_IfdCard)
#include "test_IfdCard.moc"
