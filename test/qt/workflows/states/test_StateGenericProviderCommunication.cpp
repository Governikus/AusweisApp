/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetSelfAuthenticationData.h"

#include "TlsChecker.h"

#include "MockNetworkManager.h"

#include <QList>
#include <QPair>
#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


using Pair = QPair<QByteArray, QByteArray>;
Q_DECLARE_METATYPE(QList<Pair>)


class StateGenericProviderCommunicationImpl
	: public StateGenericProviderCommunication
{
	Q_OBJECT

	public:
		explicit StateGenericProviderCommunicationImpl(const QSharedPointer<WorkflowContext>& pContext)
			: StateGenericProviderCommunication(pContext)
		{
		}


		QUrl getRequestUrl() const override
		{
			return QUrl();
		}


		void handleNetworkReply(const QByteArray& pContent) override
		{
			Q_UNUSED(pContent)
		}


};


class test_StateGenericProviderCommunication
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<StateGenericProviderCommunicationImpl> mState;
		QSharedPointer<AuthContext> mContext;
		QSharedPointer<MockNetworkManager> mNetworkManager;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new AuthContext());
			mState.reset(StateBuilder::createState<StateGenericProviderCommunicationImpl>(mContext));
			mState->onEntry(nullptr);

			mNetworkManager.reset(new MockNetworkManager());
			Env::set(NetworkManager::staticMetaObject, mNetworkManager.data());
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
		}


		void test_ReportCommunicationError()
		{
			mState->mReply.reset(new MockNetworkReply(), &QObject::deleteLater);

			QSignalSpy spy(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			QTest::ignoreMessage(QtCriticalMsg, "Unknown_Error | \"An unexpected error has occurred during processing.\"");
			QTest::ignoreMessage(QtDebugMsg, "Operation aborted");
			mState->reportCommunicationError(GlobalStatus::Code::Unknown_Error, FailureCode::Reason::Generic_Provider_Communication_Network_Error);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Unknown_Error);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Generic_Provider_Communication_Network_Error);
			QCOMPARE(spy.count(), 1);

			mContext->setStatus(GlobalStatus::Code::No_Error);

			QTest::ignoreMessage(QtCriticalMsg, "Card_Not_Found | \"Card does not exist\"");
			QTest::ignoreMessage(QtDebugMsg, "Operation aborted");
			QTest::ignoreMessage(QtWarningMsg, "FailureCode already set to Generic_Provider_Communication_Network_Error - ignoring Generic_Provider_Communication_Certificate_Error");
			mState->reportCommunicationError(GlobalStatus::Code::Card_Not_Found, FailureCode::Reason::Generic_Provider_Communication_Certificate_Error);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Card_Not_Found);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Generic_Provider_Communication_Network_Error);
			QCOMPARE(spy.count(), 2);
		}


		void test_OnSslHandshakeDone()
		{
			mState->mReply.reset(new MockNetworkReply(), &QObject::deleteLater);

			QTest::ignoreMessage(QtDebugMsg, "Operation aborted");
			mState->onSslHandshakeDone();

			QTest::ignoreMessage(QtInfoMsg, QRegularExpression("^Used session cipher"_L1));
			mState->mReply->setSslConfiguration(QSslConfiguration());
			mState->onSslHandshakeDone();
		}


		void test_OnSslErrors()
		{
			mState->mReply.reset(new MockNetworkReply(), &QObject::deleteLater);
			QSignalSpy spy(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			mState->onSslErrors(QList<QSslError>());
			QCOMPARE(spy.count(), 0);

			const QList<QSslError> errorList({QSslError(QSslError::CertificateNotYetValid), QSslError(QSslError::CertificateUntrusted)});
			mState->onSslErrors(errorList);
			QCOMPARE(spy.count(), 1);

			const auto& failureCode = mState->getContext()->getFailureCode();
			QCOMPARE(failureCode, FailureCode::Reason::Generic_Provider_Communication_Tls_Error);
			QCOMPARE(failureCode->getFailureInfoMap()[FailureCode::Info::Network_Error], mState->mReply->errorString());
			QCOMPARE(failureCode->getFailureInfoMap()[FailureCode::Info::Ssl_Errors], TlsChecker::sslErrorsToString(errorList));
		}


		void logging_data()
		{
			QTest::addColumn<QList<Pair>>("attributes");
			QTest::addColumn<bool>("enabled");

			QTest::newRow("no attr") << QList<Pair>() << true;

			QTest::newRow("single attr lower-case single") << QList<Pair>({
						{QByteArray("pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("single attr upper-case single") << QList<Pair>({
						{QByteArray("Pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("single attr lower-case multi") << QList<Pair>({
						{QByteArray("pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("single attr upper-case multi") << QList<Pair>({
						{QByteArray("Pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("single attr other") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")}
					}) << true;

			QTest::newRow("multi attr lower-case single") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("multi attr upper-case single") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("Pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("multi attr lower-case multi") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("multi attr upper-case multi") << QList<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("Pragma"), QByteArray("no-log no-cache")}
					}) << false;
		}


		void logging()
		{
			QFETCH(QList<Pair>, attributes);
			QFETCH(bool, enabled);

			MockNetworkReply* reply = new MockNetworkReply(QByteArrayLiteral("TEST"));
			for (const auto& attribute : attributes)
			{
				reply->setRawHeader(attribute.first, attribute.second);
			}
			mNetworkManager->setNextReply(reply);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Status Code: 200 \"OK\""_L1));
			if (enabled)
			{
				QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Received raw data:\n TEST"_L1));
			}
			else
			{
				QTest::ignoreMessage(QtDebugMsg, QRegularExpression("no-log was requested, skip logging of raw data"_L1));
			}

			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1);     // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();
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
			QTest::addRow("200") << QNetworkReply::NetworkError::NoError << 200 << GlobalStatus::Code::No_Error << FailureCode::Reason::Generic_Provider_Communication_Network_Error;
		}


		void test_OnNetworkReplyWrongHttpStatus()
		{
			QFETCH(QNetworkReply::NetworkError, networkError);
			QFETCH(int, httpStatus);
			QFETCH(GlobalStatus::Code, globalStatusCode);
			QFETCH(FailureCode::Reason, failureCodeReason);

			auto* reply = new MockNetworkReply();
			mState->mReply.reset(reply, &QObject::deleteLater);
			reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, httpStatus);
			reply->setError(networkError, QString());

			QSignalSpy spyAbort(mState.data(), &StateGetSelfAuthenticationData::fireAbort);

			mState->onNetworkReply();

			if (networkError == QNetworkReply::NetworkError::NoError)
			{
				QCOMPARE(spyAbort.count(), 0);
				QCOMPARE(mState->getContext()->getStatus().getStatusCode(), globalStatusCode);
				return;
			}
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getStatus().getStatusCode(), globalStatusCode);
			const FailureCode::FailureInfoMap infoMap {
				{FailureCode::Info::State_Name, "StateGenericProviderCommunicationImpl"_L1},
				{FailureCode::Info::Http_Status_Code, QString::number(httpStatus)},
				{FailureCode::Info::Network_Error, "Unknown error"_L1}
			};
			const FailureCode failureCode(failureCodeReason, infoMap);
			QCOMPARE(mState->getContext()->getFailureCode(), failureCode);
			QVERIFY(mState->getContext()->getFailureCode()->getFailureInfoMap() == infoMap);
		}


};

QTEST_GUILESS_MAIN(test_StateGenericProviderCommunication)
#include "test_StateGenericProviderCommunication.moc"
