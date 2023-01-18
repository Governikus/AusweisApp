/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePreparePersonalization.h"

#include "LogHandler.h"
#include "ResourceLoader.h"
#include "context/PersonalizationContext.h"

#include "MockNetworkManager.h"
#include "TestFileHelper.h"

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


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
			mState.reset(new StatePreparePersonalization(mContext));

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
			QCOMPARE(url, QString("https://dummy/preparePersonalization"));

		}


		void test_CheckPayload()
		{
			mContext->setSessionIdentifier(QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"));
			mContext->setPreparePersonalizationData(QString("UHJlcGFyZVBlcnNvbmFsaXphdGlvbkRhdGE="));

			const auto& payload = mState->getPayload();
			QJsonParseError jsonError {};
			const auto& json = QJsonDocument::fromJson(payload, &jsonError);
			QCOMPARE(jsonError.error, QJsonParseError::NoError);

			const auto obj = json.object();
			QCOMPARE(obj.size(), 2);
			QCOMPARE(obj.value(QLatin1String("sessionID")).toString(), QString("135a32d8-ccfa-11eb-b8bc-0242ac130003"));
			QCOMPARE(obj.value(QLatin1String("preparePersonalizationData")).toString(), QString("UHJlcGFyZVBlcnNvbmFsaXphdGlvbkRhdGE="));
		}


		void test_OnNetworkReplySuccess()
		{
			mState->mReply.reset(new MockNetworkReply(), &QObject::deleteLater);
			QSignalSpy spyContinue(mState.data(), &StatePreparePersonalization::fireContinue);

			mState->onNetworkReply();
			QCOMPARE(spyContinue.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::No_Error);
		}


		void test_OnNetworkReplyWrongHttpStatus()
		{
			auto reply = new MockNetworkReply();
			mState->mReply.reset(reply, &QObject::deleteLater);
			reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, 500);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSignalSpy spyAbort(mState.data(), &StatePreparePersonalization::fireAbort);

			mState->onNetworkReply();
			const QString logMsg(logSpy.takeLast().at(0).toString());
			QVERIFY(logMsg.contains("Network request failed"));
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
		}


		void test_AvoidLoggingOfTheRequest()
		{
			mContext->setSessionIdentifier(QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"));
			mContext->setPreparePersonalizationData(QString("data containing the PIN when Smart-eID is of type HWKeyStore"));
			mState->setStateName(QStringLiteral("StatePreparePersonalization"));
			mState->onEntry(nullptr);
			mNetworkManager->setNextReply(new MockNetworkReply(QByteArrayLiteral("TEST")));

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
