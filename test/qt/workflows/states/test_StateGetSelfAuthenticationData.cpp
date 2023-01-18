/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetSelfAuthenticationData.h"

#include "LogHandler.h"
#include "context/SelfAuthContext.h"

#include "MockNetworkReply.h"
#include "TestFileHelper.h"

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


class test_StateGetSelfAuthenticationData
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateGetSelfAuthenticationData> mState;
	QSharedPointer<SelfAuthContext> mContext;

	private Q_SLOTS:
		void init()
		{
			Env::getSingleton<LogHandler>()->init();
			mContext.reset(new SelfAuthContext());
			mState.reset(new StateGetSelfAuthenticationData(mContext));
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
		}


		void test_OnNetworkReplyNoValidData()
		{
			mState->mReply.reset(new MockNetworkReply(), &QObject::deleteLater);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSignalSpy spyAbort(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			mState->onNetworkReply();
			const QString logMsg(logSpy.takeLast().at(0).toString());
			QVERIFY(logMsg.contains("No valid data of self-authentication."));
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
		}


		void test_OnNetworkReplyValidData()
		{
			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationData.json");
			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);
			QSignalSpy spyContinue(mState.data(), &StateGetSelfAuthenticationData::fireContinue);

			mState->onNetworkReply();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_OnNetworkReplyWrongHttpStatus()
		{
			auto reply = new MockNetworkReply();
			mState->mReply.reset(reply, &QObject::deleteLater);
			reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, 500);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSignalSpy spyAbort(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			mState->onNetworkReply();
			const QString logMsg(logSpy.takeLast().at(0).toString());
			QVERIFY(logMsg.contains("Network request failed"));
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
		}


		void test_SetProgress()
		{
			mState->setProgress();
			QCOMPARE(mContext->getProgressValue(), 100);
			QCOMPARE(mContext->getProgressMessage(), tr("Getting results from server"));
		}


};

QTEST_GUILESS_MAIN(test_StateGetSelfAuthenticationData)
#include "test_StateGetSelfAuthenticationData.moc"
