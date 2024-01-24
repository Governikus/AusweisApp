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
			mState.reset(StateBuilder::createState<StateGetSessionId>(mContext));
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
			QTest::addColumn<QString>("logMsg");

			QTest::newRow("empty") << QByteArray() << QString("JSON parsing failed: \"illegal value\"");
			QTest::newRow("empty object") << QByteArray(R"({
			})") << QString("No valid sessionID to prepare personalization");
			QTest::newRow("wrong sessionID type") << QByteArray(R"({
				"sessionID": 0
			})") << QString("No valid sessionID to prepare personalization");
			QTest::newRow("wrong sessionID value") << QByteArray(R"({
				"sessionID": "HelloWorld!"
			})") << QString("No valid sessionID to prepare personalization");
		}


		void test_OnNetworkReplyNoValidData()
		{
			QFETCH(QByteArray, data);
			QFETCH(QString, logMsg);

			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);

			QSignalSpy spyAbort(mState.data(), &StateGetSessionId::fireAbort);

			const auto bytes = logMsg.toUtf8();
			QTest::ignoreMessage(QtDebugMsg, bytes.data());
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_Authentication_Failed);
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
			QTest::newRow("additional data") << QByteArray(R"({
				"statusCode": 0,
				"sessionID": "{135a32d8-ccfa-11eb-b8bc-0242ac130003}"
			})");
		}


		void test_OnNetworkReplyValidData()
		{
			QFETCH(QByteArray, data);

			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
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
			reply->setError(QNetworkReply::NetworkError::InternalServerError, QString());

			QSignalSpy spyAbort(mState.data(), &StateGetSessionId::fireAbort);

			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_TrustedChannel_Server_Error);
			const FailureCode::FailureInfoMap infoMap {
				{FailureCode::Info::State_Name, "StateGetSessionId"},
				{FailureCode::Info::Http_Status_Code, QString::number(500)},
				{FailureCode::Info::Network_Error, "Unknown error"}
			};
			const FailureCode failureCode(FailureCode::Reason::Generic_Provider_Communication_Server_Error, infoMap);
			QCOMPARE(mState->getContext()->getFailureCode(), failureCode);
			QCOMPARE(mState->getContext()->getFailureCode()->getFailureInfoMap(), infoMap);
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
