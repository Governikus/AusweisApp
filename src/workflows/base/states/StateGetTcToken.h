/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

#include <QNetworkReply>
#include <QSharedPointer>

class test_StateGetTcToken;

namespace governikus
{

class StateGetTcToken
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateGetTcToken;

	private:
		QSharedPointer<QNetworkReply> mReply;

		void parseTcToken();
		void sendRequest(const QUrl& pUrl);
		bool isValidRedirectUrl(const QUrl& pUrl);
		void run() override;

		explicit StateGetTcToken(const QSharedPointer<WorkflowContext>& pContext);

	private Q_SLOTS:
		void onNetworkReply();
		void onSslHandshakeDone();
		void onSslErrors(const QList<QSslError>& pErrors);

	public:
		void onExit(QEvent* pEvent) override;
};

} // namespace governikus
