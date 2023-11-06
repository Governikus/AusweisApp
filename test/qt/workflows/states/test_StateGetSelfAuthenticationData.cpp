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
			mState.reset(StateBuilder::createState<StateGetSelfAuthenticationData>(mContext));
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


		void test_OnNetworkReplyWrongHttpStatus_data()
		{
			QTest::addColumn<QNetworkReply::NetworkError>("networkError");
			QTest::addColumn<int>("httpStatus");
			QTest::addColumn<GlobalStatus::Code>("globalStatusCode");
			QTest::addColumn<FailureCode::Reason>("failureCodeReason");

			QTest::addRow("503") << QNetworkReply::NetworkError::ServiceUnavailableError << 503 << GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable << FailureCode::Reason::Generic_Provider_Communication_ServiceUnavailable;
			QTest::addRow("500") << QNetworkReply::NetworkError::InternalServerError << 500 << GlobalStatus::Code::Workflow_TrustedChannel_Server_Error << FailureCode::Reason::Generic_Provider_Communication_Server_Error;
			QTest::addRow("400") << QNetworkReply::NetworkError::ProtocolInvalidOperationError << 400 << GlobalStatus::Code::Workflow_TrustedChannel_Client_Error << FailureCode::Reason::Generic_Provider_Communication_Client_Error;
			QTest::addRow("404") << QNetworkReply::NetworkError::ContentNotFoundError << 404 << GlobalStatus::Code::Workflow_TrustedChannel_Client_Error << FailureCode::Reason::Generic_Provider_Communication_Client_Error;
			QTest::addRow("302") << QNetworkReply::NetworkError::ProtocolUnknownError << 302 << GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error << FailureCode::Reason::Generic_Provider_Communication_Network_Error;
			QTest::addRow("200") << QNetworkReply::NetworkError::NoError << 200 << GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided << FailureCode::Reason::Get_SelfAuthData_Invalid_Or_Empty;
		}


		void test_OnNetworkReplyWrongHttpStatus()
		{
			QFETCH(QNetworkReply::NetworkError, networkError);
			QFETCH(int, httpStatus);
			QFETCH(GlobalStatus::Code, globalStatusCode);
			QFETCH(FailureCode::Reason, failureCodeReason);

			auto reply = new MockNetworkReply();
			mState->mReply.reset(reply, &QObject::deleteLater);
			reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, httpStatus);
			reply->setError(networkError, QString());

			QSignalSpy spyAbort(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			mState->onNetworkReply();

			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus().getStatusCode(), globalStatusCode);
			const FailureCode::FailureInfoMap infoMap {
				{FailureCode::Info::State_Name, "StateGetSelfAuthenticationData"},
				{FailureCode::Info::Http_Status_Code, QString::number(httpStatus)},
				{FailureCode::Info::Network_Error, "Unknown error"}
			};
			const FailureCode failureCode(failureCodeReason, infoMap);
			QCOMPARE(mState->getContext()->getFailureCode(), failureCode);
			if (httpStatus != 200)
			{
				QVERIFY(mState->getContext()->getFailureCode()->getFailureInfoMap() == infoMap);
			}
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
