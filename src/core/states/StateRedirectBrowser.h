/*!
 * \brief Send a redirect to the browser.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "ECardApiResult.h"
#include "states/AbstractGenericState.h"

#include <http_parser.h>

namespace governikus
{

class StateRedirectBrowser
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateRedirectBrowser(const QSharedPointer<WorkflowContext>& pContext);

	void reportCommunicationError();
	void sendErrorPage(http_status pStatus);
	bool sendRedirect(const QUrl& pRedirectAddress, const ECardApiResult& pResult);
	virtual void run() override;

};

} // namespace governikus
