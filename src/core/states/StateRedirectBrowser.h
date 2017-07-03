/*!
 * StateRedirectBrowser.h
 *
 * \brief Send a redirect to the browser.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "Result.h"
#include "context/AuthContext.h"
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
	bool sendRedirect(const QUrl& pRedirectAddress, const Result& pResult);
	virtual void run() override;

};

} /* namespace governikus */
