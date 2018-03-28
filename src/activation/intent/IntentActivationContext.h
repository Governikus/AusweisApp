/*!
 * \brief Implementation of ActivationContext for Intent
 * based activation on Android systems.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationContext.h"


namespace governikus
{

class IntentActivationContext
	: public ActivationContext
{
	Q_OBJECT

	const QUrl mActivationUrl;
	QUrl mRedirectAddress;

	public:
		IntentActivationContext(const QUrl& pActivationUrl);
		virtual ~IntentActivationContext() override;

		QUrl getActivationURL() const override;

		bool sendProcessing() override;
		bool sendOperationAlreadyActive() override;
		bool sendErrorPage(HttpStatusCode pStatusCode, const GlobalStatus& pStatus) override;
		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pResult) override;

	Q_SIGNALS:
		void fireShowUserInformation(const QString& pMessage);
};

} /* namespace governikus */
