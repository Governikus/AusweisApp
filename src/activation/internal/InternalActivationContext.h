/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
		virtual ~InternalActivationContext();

		QUrl getActivationURL() override;
		bool sendProcessing() override;
		bool sendOperationAlreadyActive() override;
		bool sendErrorPage(HttpStatusCode pStatusCode, const Result& pResult) override;
		bool sendRedirect(const QUrl& pRedirectAddress, const Result& pResult) override;
};

} /* namespace governikus */
