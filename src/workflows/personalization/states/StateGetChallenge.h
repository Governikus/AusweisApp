/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief get the xml data from the given url
 */

#pragma once

#include "states/StateGenericProviderCommunication.h"


class test_StateGetChallenge;


namespace governikus
{

class StateGetChallenge
	: public StateGenericProviderCommunication
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateGetChallenge;

	private:
		explicit StateGetChallenge(const QSharedPointer<WorkflowContext>& pContext);

		[[nodiscard]] QUrl getRequestUrl() const override;
		[[nodiscard]] QByteArray getPayload() const override;
		void setProgress() const override;

		[[nodiscard]] bool parseChallenge(const QByteArray& pData);

		void handleNetworkReply(const QByteArray& pContent) override;
};

} // namespace governikus
