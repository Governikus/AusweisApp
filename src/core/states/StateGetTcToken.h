/*!
 * \brief State machine state: Get TCToken.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QNetworkReply>
#include <QPointer>

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

class test_StateGetTcToken;

namespace governikus
{

class StateGetTcToken
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateGetTcToken;

	QPointer<QNetworkReply> mReply;

	void parseTcToken();
	void sendRequest(const QUrl& pUrl);
	bool isValidRedirectUrl(const QUrl& pUrl);
	virtual void run() override;

	explicit StateGetTcToken(const QSharedPointer<WorkflowContext>& pContext);

	public:
		virtual ~StateGetTcToken() override;

	private Q_SLOTS:
		void onNetworkReply();
		void onSslHandshakeDone();
		void onSslErrors(const QList<QSslError>& pErrors);
};

} // namespace governikus
