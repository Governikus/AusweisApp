/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationContext.h"

namespace governikus
{

class InternalActivationContext
	: public ActivationContext
{
	Q_OBJECT

	private:
		const QUrl mTcTokenUrl;

	public:
		InternalActivationContext(const QUrl& pUrl);
		virtual ~InternalActivationContext() override;

		QUrl getActivationURL() const override;
		bool sendProcessing() override;
		bool sendOperationAlreadyActive() override;
		bool sendErrorPage(HttpStatusCode pStatusCode, const GlobalStatus& pStatus) override;
		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) override;
};

} /* namespace governikus */
