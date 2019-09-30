/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetSelfAuthenticationData.h"

#include "HttpServer.h"
#include "LogHandler.h"

#include "MockNetworkReply.h"
#include "TestAuthContext.h"
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


		void test_ReportCommunicationError()
		{
			MockNetworkReply reply;
			mState->mReply = QPointer<QNetworkReply>(&reply);

			QSignalSpy spy(mState.data(), &StateGetSelfAuthenticationData::fireAbort);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			mState->reportCommunicationError(GlobalStatus::Code::Unknown_Error);
			QVERIFY(logSpy.takeLast().at(0).toString().contains("Operation aborted"));
			QVERIFY(logSpy.takeLast().at(0).toString().contains("Unknown_Error | \"An unexpected error has occurred during processing.\""));
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Unknown_Error);
			QCOMPARE(spy.count(), 1);

			mContext->setStatus(GlobalStatus::Code::No_Error);

			mState->reportCommunicationError(GlobalStatus::Code::Card_Not_Found);
			QVERIFY(logSpy.takeLast().at(0).toString().contains("Operation aborted"));
			QVERIFY(logSpy.takeLast().at(0).toString().contains("Card_Not_Found | \"Card does not exist\""));
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Card_Not_Found);
			QCOMPARE(spy.count(), 2);
		}


		void test_OnSslHandshakeDone()
		{
			MockNetworkReply reply;
			mState->mReply = QPointer<QNetworkReply>(&reply);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			mState->onSslHandshakeDone();
			QVERIFY(logSpy.takeLast().at(0).toString().contains("Operation aborted"));

			reply.setSslConfiguration(QSslConfiguration());
			mState->onSslHandshakeDone();
			const QString logMsg(logSpy.at(0).at(0).toString());
			QVERIFY(logMsg.contains("Used session cipher"));
		}


		void test_OnSslErrors()
		{
			MockNetworkReply reply;
			mState->mReply = QPointer<QNetworkReply>(&reply);
			QSignalSpy spy(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			mState->onSslErrors(QList<QSslError>());
			QCOMPARE(spy.count(), 0);

			const QList<QSslError> errorList = {QSslError::CertificateNotYetValid, QSslError::CertificateUntrusted};
			mState->onSslErrors(errorList);
			QCOMPARE(spy.count(), 1);
		}


		void test_OnNetworkReplyNoValidData()
		{
			MockNetworkReply emptyReply;
			mState->mReply = QPointer<QNetworkReply>(&emptyReply);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
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
			MockNetworkReply reply(data);
			mState->mReply = QPointer<QNetworkReply>(&reply);
			QSignalSpy spyContinue(mState.data(), &StateGetSelfAuthenticationData::fireContinue);

			mState->onNetworkReply();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_OnNetworkReplyWrongHttpStatus()
		{
			MockNetworkReply reply;
			reply.setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, 500);
			mState->mReply = QPointer<QNetworkReply>(&reply);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			QSignalSpy spyAbort(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			mState->onNetworkReply();
			const QString logMsg(logSpy.takeLast().at(0).toString());
			QVERIFY(logMsg.contains("Could not read data for self-authentication."));
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus(), GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
		}


};

QTEST_GUILESS_MAIN(test_StateGetSelfAuthenticationData)
#include "test_StateGetSelfAuthenticationData.moc"
