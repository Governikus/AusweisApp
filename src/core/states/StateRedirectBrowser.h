/*!
 * StateRedirectBrowser.h
 *
 * \brief Send a redirect to the browser.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "Result.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateRedirectBrowser
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateRedirectBrowser(const QSharedPointer<WorkflowContext>& pContext);

	void reportCommunicationError();
	void sendErrorPage(HttpStatusCode pStatus);
	bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus);
	virtual void run() override;

};

} /* namespace governikus */
