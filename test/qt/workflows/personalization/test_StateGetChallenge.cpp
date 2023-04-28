/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetChallenge.h"

#include "ResourceLoader.h"
#include "context/PersonalizationContext.h"

#include "MockNetworkReply.h"

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


class test_StateGetChallenge
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateGetChallenge> mState;
	QSharedPointer<PersonalizationContext> mContext;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mContext.reset(new PersonalizationContext(QStringLiteral("https://dummy/v1/%1")));
			mState.reset(new StateGetChallenge(mContext));
			mState->setStateName("StateGetChallenge");
			QVERIFY(mContext->getChallenge().isEmpty());
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
		}


		void test_CheckRequestUrl()
		{
			mContext->setSessionIdentifier(QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"));

			const auto& url = mState->getRequestUrl().toString();
			QCOMPARE(url, QString("https://dummy/v1/challenge"));
		}


		void test_CheckPayload()
		{
			mContext->setSessionIdentifier(QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"));

			const auto& payload = mState->getPayload();
			QJsonParseError jsonError {};
			const auto& json = QJsonDocument::fromJson(payload, &jsonError);
			QCOMPARE(jsonError.error, QJsonParseError::NoError);

			const auto obj = json.object();
			QCOMPARE(obj.size(), 2);
			QCOMPARE(obj.value(QLatin1String("sessionID")).toString(), QString("135a32d8-ccfa-11eb-b8bc-0242ac130003"));
			QCOMPARE(obj.value(QLatin1String("osType")).toString(), QString("Unknown"));
		}


		void test_OnNetworkReplyNoValidData()
		{
			mState->mReply.reset(new MockNetworkReply(), &QObject::deleteLater);

			QSignalSpy spyAbort(mState.data(), &StateGetChallenge::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "No valid challenge to prepare personalization");
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			QCOMPARE(mState->getContext()->getFailureCode() == FailureCode::Reason::Get_Challenge_Invalid, true);
			QVERIFY(mContext->getChallenge().isEmpty());
		}


		void test_OnNetworkReplyValidData()
		{
			const QByteArray data(R"({
				"challenge": "Q2hhbGxlbmdl"
			})");
			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);
			QSignalSpy spyContinue(mState.data(), &StateGetChallenge::fireContinue);

			mState->onNetworkReply();
			QCOMPARE(spyContinue.count(), 1);
			QCOMPARE(mContext->getChallenge(), QString("Q2hhbGxlbmdl"));
		}


		void test_OnNetworkReplyWrongHttpStatus()
		{
			auto reply = new MockNetworkReply();
			mState->mReply.reset(reply, &QObject::deleteLater);
			reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, 500);

			QSignalSpy spyAbort(mState.data(), &StateGetChallenge::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Network request failed");
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);

			const FailureCode::FailureInfoMap infoMap {
				{FailureCode::Info::State_Name, "StateGetChallenge"},
				{FailureCode::Info::Http_Status_Code, QString::number(500)},
				{FailureCode::Info::Network_Error, "Unknown error"}
			};
			const FailureCode failureCode(FailureCode::Reason::Generic_Provider_Communication_Network_Error, infoMap);
			QCOMPARE(mState->getContext()->getFailureCode() == failureCode, true);
			QVERIFY(mState->getContext()->getFailureCode()->getFailureInfoMap() == infoMap);

			QVERIFY(mContext->getChallenge().isEmpty());
		}


		void test_SetProgress()
		{
			mState->setProgress();
			QCOMPARE(mContext->getProgressValue(), 10);
			QCOMPARE(mContext->getProgressMessage(), tr("Getting challenge from server"));
		}


};

QTEST_GUILESS_MAIN(test_StateGetChallenge)
#include "test_StateGetChallenge.moc"
