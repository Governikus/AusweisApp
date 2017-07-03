/*!
 * CustomSchemeActivationContext.h
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "ActivationContext.h"

namespace governikus
{

class CustomSchemeActivationContext
	: public ActivationContext
{
	Q_OBJECT

	private:
		const QUrl mActivationUrl;
		QUrl mRedirectAddress;

	public:
		CustomSchemeActivationContext(const QUrl& pActivationUrl);

		virtual ~CustomSchemeActivationContext();

		QUrl getActivationURL() override;

		bool sendProcessing() override;

		bool sendOperationAlreadyActive() override;

		bool sendErrorPage(HttpStatusCode pStatusCode, const Result& pResult) override;

		bool sendRedirect(const QUrl& pRedirectAddress, const Result& pResult) override;
};

} /* namespace governikus */
