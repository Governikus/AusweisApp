/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Calls the RefreshAddress of TcToken and checks the certificates.
 * After that it will set RedirectAddress in WorkflowContext.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

#include <QNetworkReply>
#include <QSharedPointer>
#include <QSslCertificate>
#include <QSslError>
#include <QSslSocket>

class test_StateCheckRefreshAddress;

namespace governikus
{

class StateCheckRefreshAddress
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateCheckRefreshAddress;

	private:
		QSharedPointer<QNetworkReply> mReply;
		QUrl mUrl;
		QUrl mSubjectUrl;
		bool mCertificateFetched;
		QList<QUrl> mVerifiedRefreshUrlHosts;

		explicit StateCheckRefreshAddress(const QSharedPointer<WorkflowContext>& pContext);

		[[nodiscard]] bool isMatchingSameOriginPolicyInDevMode() const;
		void run() override;

		QUrl determineSubjectUrl() const;

		void sendGetRequest();
		void fetchServerCertificate();
		bool checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration);
		void doneSuccess();
		void reportCommunicationError(const GlobalStatus& pStatus, const FailureCode& pFailure);

	private Q_SLOTS:
		void onSslHandshakeDone();
		void onNetworkReply();
		void onSslErrors(const QList<QSslError>& errors);
		void onSslHandshakeDoneFetchingServerCertificate();
		void onNetworkErrorFetchingServerCertificate(QNetworkReply::NetworkError pError);

	public:
		void onEntry(QEvent* pEvent) override;
		void onExit(QEvent* pEvent) override;
};

} // namespace governikus
