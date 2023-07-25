/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetSelfAuthenticationData.h"

#include "TlsChecker.h"

#include "MockNetworkManager.h"

#include <QPair>
#include <QVector>
#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


using Pair = QPair<QByteArray, QByteArray>;
Q_DECLARE_METATYPE(QVector<Pair>)


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
			mContext.reset(new AuthContext(QSharedPointer<ActivationContext>()));
			mState.reset(new StateGenericProviderCommunicationImpl(mContext));
			mState->setStateName("StateGenericProviderCommunicationImpl");
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

			QTest::ignoreMessage(QtInfoMsg, QRegularExpression("^Used session cipher"));
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
			QTest::addColumn<QVector<Pair>>("attributes");
			QTest::addColumn<bool>("enabled");

			QTest::newRow("no attr") << QVector<Pair>() << true;

			QTest::newRow("single attr lower-case single") << QVector<Pair>({
						{QByteArray("pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("single attr upper-case single") << QVector<Pair>({
						{QByteArray("Pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("single attr lower-case multi") << QVector<Pair>({
						{QByteArray("pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("single attr upper-case multi") << QVector<Pair>({
						{QByteArray("Pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("single attr other") << QVector<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")}
					}) << true;

			QTest::newRow("multi attr lower-case single") << QVector<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("multi attr upper-case single") << QVector<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("Pragma"), QByteArray("no-log")}
					}) << false;
			QTest::newRow("multi attr lower-case multi") << QVector<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("pragma"), QByteArray("no-log no-cache")}
					}) << false;
			QTest::newRow("multi attr upper-case multi") << QVector<Pair>({
						{QByteArray("Connection"), QByteArray("keep-alive")},
						{QByteArray("Pragma"), QByteArray("no-log no-cache")}
					}) << false;
		}


		void logging()
		{
			QFETCH(QVector<Pair>, attributes);
			QFETCH(bool, enabled);

			MockNetworkReply* reply = new MockNetworkReply(QByteArrayLiteral("TEST"));
			for (const auto& attribute : attributes)
			{
				reply->setRawHeader(attribute.first, attribute.second);
			}
			mNetworkManager->setNextReply(reply);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Status Code: 200 \"OK\""));
			if (enabled)
			{
				QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Received raw data:\n TEST"));
			}
			else
			{
				QTest::ignoreMessage(QtDebugMsg, QRegularExpression("no-log was requested, skip logging of raw data"));
			}

			QSignalSpy spyMock(mNetworkManager.data(), &MockNetworkManager::fireReply);

			mContext->setStateApproved();
			QTRY_COMPARE(spyMock.count(), 1);     // clazy:exclude=qstring-allocations
			mNetworkManager->fireFinished();
		}


};

QTEST_GUILESS_MAIN(test_StateGenericProviderCommunication)
#include "test_StateGenericProviderCommunication.moc"
