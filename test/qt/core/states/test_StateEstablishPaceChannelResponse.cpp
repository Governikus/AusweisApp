/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/remote_service/StateEstablishPaceChannelResponse.h"

#include "states/StateBuilder.h"

#include "MockDataChannel.h"
#include "MockRemoteServer.h"

#include <QtTest>

using namespace governikus;

class MockServerMsgHandler
	: public ServerMessageHandler
{
	Q_OBJECT
	QString mResponse;

	public:
		explicit MockServerMsgHandler()
			: ServerMessageHandler()
			, mResponse()
		{
		}


		virtual void sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pChannelOutput) override
		{
			Q_UNUSED(pChannelOutput);
			mResponse = pSlotHandle;
		}


		virtual void sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu) override
		{
			Q_UNUSED(pSlotHandle);
			Q_UNUSED(pResponseApdu);
		}


		QString getResponse()
		{
			return mResponse;
		}


};

class test_StateEstablishPaceChannelResponse
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateEstablishPaceChannelResponse> mState;
	QSharedPointer<RemoteServiceContext> mContext;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::setCreator<RemoteServer*>(std::function<RemoteServer*()>([&] {
						return new MockRemoteServer();
					}));
		}


		void init()
		{
			mContext.reset(new RemoteServiceContext());
			mState.reset(StateBuilder::createState<StateEstablishPaceChannelResponse>(mContext));
		}


		void cleanup()
		{
			mContext.clear();
			mState.clear();
		}


		void test_Run_data()
		{
			QTest::addColumn<QByteArray>("hexBytes");
			QTest::addColumn<int>("wrongPacePasswordCounter");
			QTest::addColumn<int>("continueCounter");

			QByteArray data1 = QByteArray("30 10"
										  "a1 06 04 04 f00663c2"
										  "a2 04 04 02 0000"
										  "a3 00"
										  "9000");
			QByteArray data2 = QByteArray("30 5A"
										  "A1 06 04 04 F0200001"
										  "A2 04 04 02 9000"
										  "A3 02 31 00"
										  "A4 22 04 20 24e41d62b8c848226b86fcc6c7657577dca47ad2bf21573617bae84807f85c6b"
										  "A5 10 04 0e 4445435643416549443030313033"
										  "A6 10 04 0e 4445435643416549443030313032"
										  "9000");

			QTest::newRow("wrongPacePassword") << data1 << 1 << 0;
			QTest::newRow("correctPacePassword") << data2 << 0 << 1;
		}


		void test_Run()
		{
			QFETCH(QByteArray, hexBytes);
			QFETCH(int, wrongPacePasswordCounter);
			QFETCH(int, continueCounter);

			QSignalSpy spyWrongPacePassword(mState.data(), &StateEstablishPaceChannelResponse::fireWrongPacePassword);
			QSignalSpy spyContinue(mState.data(), &StateEstablishPaceChannelResponse::fireContinue);

			const QString slotHandle("slot handle");
			const QSharedPointer<const IfdEstablishPaceChannel> msg(new IfdEstablishPaceChannel(slotHandle));
			mContext->setEstablishPaceChannelMessage(msg);
			EstablishPaceChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PacePasswordId::PACE_PIN);
			mContext->setEstablishPaceChannelOutput(channelOutput);
			const QSharedPointer<MockServerMsgHandler> msgHandler(new MockServerMsgHandler());
			auto server = mContext->getRemoteServer().staticCast<MockRemoteServer>();
			server->setMessageHandler(msgHandler);

			mContext->setStateApproved();
			mState->onStateApprovedChanged();
			QCOMPARE(spyWrongPacePassword.count(), wrongPacePasswordCounter);
			QCOMPARE(spyContinue.count(), continueCounter);
			QCOMPARE(msgHandler->getResponse(), slotHandle);
		}


};

QTEST_GUILESS_MAIN(test_StateEstablishPaceChannelResponse)
#include "test_StateEstablishPaceChannelResponse.moc"
