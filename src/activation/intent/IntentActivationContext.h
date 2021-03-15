/*!
 * \brief Implementation of ActivationContext for Intent
 * based activation on Android systems.
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
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
	const QString mReferrer;
	QUrl mRedirectAddress;

	public:
		explicit IntentActivationContext(const QUrl& pActivationUrl, const QString& pReferrer);
		~IntentActivationContext() override;

		[[nodiscard]] QUrl getActivationURL() const override;

		bool sendProcessing() override;
		bool sendOperationAlreadyActive() override;
		bool sendErrorPage(http_status pStatusCode, const GlobalStatus& pStatus) override;
		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) override;

	Q_SIGNALS:
		void fireShowUserInformation(const QString& pMessage);
};

} // namespace governikus
