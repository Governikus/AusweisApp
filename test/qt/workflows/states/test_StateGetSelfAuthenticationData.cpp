/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetSelfAuthenticationData.h"

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
			mContext.reset(new SelfAuthContext());
			mState.reset(new StateGetSelfAuthenticationData(mContext));
			mState->setStateName("StateGetSelfAuthenticationData");
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
		}


		void test_OnNetworkReplyNoValidData()
		{
			mState->mReply.reset(new MockNetworkReply(), &QObject::deleteLater);

			QSignalSpy spyAbort(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "No valid data of self-authentication.");
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			QCOMPARE(mState->getContext()->getFailureCode(), FailureCode::Reason::Get_SelfAuthData_Invalid_Or_Empty);
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

			QSignalSpy spyAbort(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Network request failed");
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			const FailureCode::FailureInfoMap infoMap {
				{FailureCode::Info::State_Name, "StateGetSelfAuthenticationData"},
				{FailureCode::Info::Http_Status_Code, QString::number(500)},
				{FailureCode::Info::Network_Error, "Unknown error"}
			};
			const FailureCode failureCode(FailureCode::Reason::Generic_Provider_Communication_Network_Error, infoMap);
			QCOMPARE(mState->getContext()->getFailureCode(), failureCode);
			QVERIFY(mState->getContext()->getFailureCode()->getFailureInfoMap() == infoMap);
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
