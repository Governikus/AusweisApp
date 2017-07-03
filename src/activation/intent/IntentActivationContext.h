/*!
 * \brief Implementation of ActivationContext for Intent
 * based activation on Android systems.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "ActivationContext.h"


namespace governikus
{

class IntentActivationContext
	: public ActivationContext
{
	const QUrl mActivationUrl;
	QUrl mRedirectAddress;

	public:
		IntentActivationContext(const QUrl& pActivationUrl);
		virtual ~IntentActivationContext();

		QUrl getActivationURL() const override;

		bool sendProcessing() override;
		bool sendOperationAlreadyActive() override;
		bool sendErrorPage(HttpStatusCode pStatusCode, const GlobalStatus& pStatus) override;
		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pResult) override;
};

} /* namespace governikus */
