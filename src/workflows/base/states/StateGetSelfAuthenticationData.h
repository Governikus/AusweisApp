/*!
 * \brief get the xml data from the given url
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
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

	private:
		QSharedPointer<QNetworkReply> mReply;

		explicit StateGetSelfAuthenticationData(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
		void reportCommunicationError(const GlobalStatus& pStatus);
		bool checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration);

	private Q_SLOTS:
		void onNetworkReply();
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();

	public:
		void onExit(QEvent* pEvent) override;
};

} // namespace governikus
