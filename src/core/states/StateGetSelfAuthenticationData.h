/*!
 * \brief get the xml data from the given url
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/SelfAuthContext.h"
#include "NetworkManager.h"
#include "Result.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateGetSelfAuthenticationData
	: public AbstractGenericState<SelfAuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	QPointer<QNetworkReply> mReply;

	StateGetSelfAuthenticationData(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
	void reportCommunicationError(const GlobalStatus& pStatus);
	bool checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration);

	public:
		virtual ~StateGetSelfAuthenticationData() override;

	private Q_SLOTS:
		void onNetworkReply();
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();
};

} /* namespace governikus */
