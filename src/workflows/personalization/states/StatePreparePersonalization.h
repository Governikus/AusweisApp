/*!
 * \brief State that initializes the pre personalization of the Smart-eID applet
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "states/StateGenericProviderCommunication.h"


class test_StatePreparePersonalization;


namespace governikus
{

class StatePreparePersonalization
	: public StateGenericProviderCommunication
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StatePreparePersonalization;

	private:
		explicit StatePreparePersonalization(const QSharedPointer<WorkflowContext>& pContext);

		[[nodiscard]] QUrl getRequestUrl() const override;
		[[nodiscard]] QByteArray getPayload() const override;

		void handleNetworkReply(const QByteArray& pContent) override;
};

} // namespace governikus
