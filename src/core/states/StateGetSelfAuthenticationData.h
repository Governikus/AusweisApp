/*!
 * \brief get the xml data from the given url
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/SelfAuthContext.h"
#include "GenericContextContainer.h"
#include "NetworkManager.h"

class test_StateGetSelfAuthenticationData;

namespace governikus
{

class StateGetSelfAuthenticationData
	: public AbstractState
	, public GenericContextContainer<SelfAuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateGetSelfAuthenticationData;

	QPointer<QNetworkReply> mReply;

	explicit StateGetSelfAuthenticationData(const QSharedPointer<WorkflowContext>& pContext);
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

} // namespace governikus
