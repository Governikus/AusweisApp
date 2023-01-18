/*!
 * \brief get the xml data from the given url
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "states/StateGenericProviderCommunication.h"


class test_StateGetSelfAuthenticationData;


namespace governikus
{

class StateGetSelfAuthenticationData
	: public StateGenericProviderCommunication
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateGetSelfAuthenticationData;

	private:
		explicit StateGetSelfAuthenticationData(const QSharedPointer<WorkflowContext>& pContext);

		QUrl getRequestUrl() const override;
		void setProgress() const override;

		void handleNetworkReply(const QByteArray& pContent) override;
		bool isLoggingAllowed() override;
};

} // namespace governikus
