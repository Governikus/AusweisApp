/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


class test_StateRedirectBrowser;


namespace governikus
{

class StateRedirectBrowser
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateRedirectBrowser;

	private:
		explicit StateRedirectBrowser(const QSharedPointer<WorkflowContext>& pContext);

		void run() override;

		static QUrl addMajorMinor(const QUrl& pUrl, const ECardApiResult& pResult);
};

} // namespace governikus
