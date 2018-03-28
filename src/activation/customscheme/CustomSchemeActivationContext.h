/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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

		QUrl getActivationURL() const override;

		bool sendProcessing() override;
		bool sendOperationAlreadyActive() override;
		bool sendErrorPage(HttpStatusCode pStatusCode, const GlobalStatus& pStatus) override;
		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pResult) override;

	Q_SIGNALS:
		void fireShowUserInformation(const QString& pMessage);
};

} /* namespace governikus */
