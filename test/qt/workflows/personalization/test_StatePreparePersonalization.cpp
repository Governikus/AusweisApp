/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePreparePersonalization.h"

#include "LogHandler.h"
#include "ResourceLoader.h"
#include "context/PersonalizationContext.h"

#include "MockNetworkManager.h"

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StatePreparePersonalization
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StatePreparePersonalization> mState;
	QSharedPointer<PersonalizationContext> mContext;
	QSharedPointer<MockNetworkManager> mNetworkManager;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			Env::getSingleton<LogHandler>()->init();
			mContext.reset(new PersonalizationContext(QStringLiteral("https://dummy/%1")));
			mState.reset(StateBuilder::createState<StatePreparePersonalization>(mContext));

			mNetworkManager.reset(new MockNetworkManager());
			Env::set(NetworkManager::staticMetaObject, mNetworkManager.data());
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
		}


		void test_CheckRequestUrl()
		{
			const auto& url = mState->getRequestUrl().toString();
			QCOMPARE(url, "https://dummy/preparePersonalization"_L1);

		}


		void test_CheckPayload()
		{
			mContext->setSessionIdentifier(QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"_L1));
			mContext->setPreparePersonalizationData("UHJlcGFyZVBlcnNvbmFsaXphdGlvbkRhdGE="_L1);

			const auto& payload = mState->getPayload();
			QJsonParseError jsonError {};
			const auto& json = QJsonDocument::fromJson(payload, &jsonError);
			QCOMPARE(jsonError.error, QJsonParseError::NoError);

			const auto obj = json.object();
			QCOMPARE(obj.size(), 2);
			QCOMPARE(obj.value(QLatin1String("sessionID")).toString(), "135a32d8-ccfa-11eb-b8bc-0242ac130003"_L1);
			QCOMPARE(obj.value(QLatin1String("preparePersonalizationData")).toString(), "UHJlcGFyZVBlcnNvbmFsaXphdGlvbkRhdGE="_L1);
		}


		void test_OnNetworkReplyNoValidData()
		{
			const QByteArray data(".");
			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);

			QSignalSpy spyAbort(mState.data(), &StatePreparePersonalization::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("No valid network response"_L1));
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			QCOMPARE(mContext->getFinalizeStatus(), 0);
		}


		void test_OnNetworkReplyValidDataFail()
		{
			const QByteArray data(R"({ "statusCode": -1 })");
			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);

			QSignalSpy spyAbort(mState.data(), &StatePreparePersonalization::fireAbort);

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("preparePersonalization failed with statusCode -1"_L1));
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mContext->getFinalizeStatus(), -1);
		}


		void test_OnNetworkReplyValidData()
		{
			const QByteArray data(R"({ "statusCode": 1 })");
			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSignalSpy spyContinue(mState.data(), &StatePreparePersonalization::fireContinue);

			mState->onNetworkReply();
			const QString logMsg(logSpy.takeLast().at(0).toString());
			QVERIFY(logMsg.contains("preparePersonalization finished with statusCode 1"_L1));
			QCOMPARE(spyContinue.count(), 1);
			QCOMPARE(mContext->getFinalizeStatus(), 1);
		}


		void test_OnNetworkReplyWrongStatusCode()
		{
			const QByteArray data(R"({ "statusCode": -2 })");
			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);

			QSignalSpy spyAbort(mState.data(), &StatePreparePersonalization::fireAbort);

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("preparePersonalization failed with statusCode -2"_L1));
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_PrePersonalization_Failed);
			QCOMPARE(mState->getContext()->getFailureCode(), FailureCode::Reason::Smart_PrePersonalization_Wrong_Status);
			QCOMPARE(mContext->getFinalizeStatus(), -2);
		}


		void test_OnNetworkReplyWrongContent()
		{
			const QByteArray data(R"({ "fooBar": 1 })");
			mState->mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);

			QSignalSpy spyAbort(mState.data(), &StatePreparePersonalization::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("JSON parsing failed: statusCode is missing"_L1));
			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			QCOMPARE(mState->getContext()->getFailureCode(), FailureCode::Reason::Smart_PrePersonalization_Incomplete_Information);
			QCOMPARE(mContext->getFinalizeStatus(), 0);
		}


		void test_OnNetworkReplyWrongHttpStatus()
		{
			auto* reply = new MockNetworkReply();
			mState->mReply.reset(reply, &QObject::deleteLater);
			reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, 500);
			reply->setError(QNetworkReply::NetworkError::InternalServerError, QString());

			QSignalSpy spyAbort(mState.data(), &StatePreparePersonalization::fireAbort);

			mState->onNetworkReply();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_TrustedChannel_Server_Error);
			const FailureCode::FailureInfoMap infoMap {
				{FailureCode::Info::State_Name, "StatePreparePersonalization"_L1},
				{FailureCode::Info::Http_Status_Code, QString::number(500)},
				{FailureCode::Info::Network_Error, "Unknown error"_L1}
			};
			const FailureCode failureCode(FailureCode::Reason::Generic_Provider_Communication_Server_Error, infoMap);
			QCOMPARE(mState->getContext()->getFailureCode(), failureCode);
			QCOMPARE(mState->getContext()->getFailureCode()->getFailureInfoMap(), infoMap);
			QCOMPARE(mContext->getFinalizeStatus(), 0);
		}


		void test_AvoidLoggingOfTheRequest()
		{
			mContext->setSessionIdentifier(QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"_L1));
			mContext->setPreparePersonalizationData("data containing the PIN when Smart-eID is of type HWKeyStore"_L1);
			mState->onEntry(nullptr);
			mNetworkManager->setNextReply(new MockNetworkReply(QByteArrayLiteral(R"({ "statusCode": 1 })")));

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);
			mContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1);     // clazy:exclude=qstring-allocations

			QSignalSpy spyContinue(mState.data(), &StatePreparePersonalization::fireContinue);
			mNetworkManager->fireFinished();
			QCOMPARE(spyContinue.count(), 1);

			for (const auto& entry : logSpy)
			{
				QVERIFY(!entry.at(0).toString().contains(QLatin1String("data containing the PIN")));
			}
		}


};

QTEST_GUILESS_MAIN(test_StatePreparePersonalization)
#include "test_StatePreparePersonalization.moc"
