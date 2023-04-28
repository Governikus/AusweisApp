/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetSessionId.h"

#include "context/PersonalizationContext.h"

#include "MockNetworkReply.h"

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


class test_StateGetSessionId
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateGetSessionId> mState;
	QSharedPointer<PersonalizationContext> mContext;

	private Q_SLOTS:
		void init()
		{
			mContext = QSharedPointer<PersonalizationContext>::create(QString());
			mState.reset(new StateGetSessionId(mContext));
			mState->setStateName("StateGetSessionId");
			QVERIFY(mContext->getSessionIdentifier().isNull());
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
		}


		void test_CheckRequestUrl()
		{
			mContext->setRefreshUrl(QUrl("https://dummy.url.de/refresh"));

			const auto& url = mState->getRequestUrl().toString();
			QCOMPARE(url, QString("https://dummy.url.de/refresh"));
		}


		void test_OnNetworkReplyNoValidData_data()
		{
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("empty") << QByteArray();
			QTest::newRow("empty object") << QByteArray(R"({
			})");
			QTest::newRow("wrong sessionID type") << QByteArray(R"({
				"sessionID": 0
			})");
			QTest::newRow("wrong sessionID value") << QByteArray(R"({
				"sessionID": "HelloWorld!"
			})");
		}


		void test_OnNetworkReplyNoValidData()
		{
			QFETCH(QByteArray, data);

			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);

			QSignalSpy spyAbort(mState.data(), &StateGetSessionId::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "No valid sessionID to prepare personalization");
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			QCOMPARE(mState->getContext()->getFailureCode(), FailureCode::Reason::Get_Session_Id_Invalid);
			QVERIFY(mContext->getSessionIdentifier().isNull());
		}


		void test_OnNetworkReplyValidData_data()
		{
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("curly braces") << QByteArray(R"({
				"sessionID": "{135a32d8-ccfa-11eb-b8bc-0242ac130003}"
			})");
			QTest::newRow("without curly braces") << QByteArray(R"({
				"sessionID": "135a32d8-ccfa-11eb-b8bc-0242ac130003"
			})");
		}


		void test_OnNetworkReplyValidData()
		{
			QFETCH(QByteArray, data);

			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);
			QSignalSpy spyContinue(mState.data(), &StateGetSessionId::fireContinue);

			mState->onNetworkReply();
			QCOMPARE(spyContinue.count(), 1);
			QCOMPARE(mContext->getSessionIdentifier(), QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"));
		}


		void test_OnNetworkReplyWrongHttpStatus()
		{
			auto reply = new MockNetworkReply();
			mState->mReply.reset(reply, &QObject::deleteLater);
			reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, 500);

			QSignalSpy spyAbort(mState.data(), &StateGetSessionId::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Network request failed");
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			const FailureCode::FailureInfoMap infoMap {
				{FailureCode::Info::State_Name, "StateGetSessionId"},
				{FailureCode::Info::Http_Status_Code, QString::number(500)},
				{FailureCode::Info::Network_Error, "Unknown error"}
			};
			const FailureCode failureCode(FailureCode::Reason::Generic_Provider_Communication_Network_Error, infoMap);
			QCOMPARE(mState->getContext()->getFailureCode() == failureCode, true);
			QVERIFY(mState->getContext()->getFailureCode()->getFailureInfoMap() == infoMap);
			QVERIFY(mContext->getSessionIdentifier().isNull());
		}


		void test_SetProgress()
		{
			mState->setProgress();
			QCOMPARE(mContext->getProgressValue(), 100);
			QCOMPARE(mContext->getProgressMessage(), tr("Getting results from server"));
		}


};

QTEST_GUILESS_MAIN(test_StateGetSessionId)
#include "test_StateGetSessionId.moc"
