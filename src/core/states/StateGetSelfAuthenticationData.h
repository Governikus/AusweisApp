/*!
 * StateGetSelfAuthenticationData.h
 *
 * \brief get the xml data from the given url
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "NetworkManager.h"
#include "Result.h"
#include "context/SelfAuthenticationContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateGetSelfAuthenticationData
	: public AbstractGenericState<SelfAuthenticationContext>
{
	Q_OBJECT
	friend class StateBuilder;

	QPointer<QNetworkReply> mReply;

	StateGetSelfAuthenticationData(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
	void reportCommunicationError(const GlobalStatus& pStatus);
	bool checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration);

	public:
		~StateGetSelfAuthenticationData();

	private Q_SLOTS:
		void onNetworkReply();
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();
};

} /* namespace governikus */
