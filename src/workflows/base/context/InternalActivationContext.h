/*!
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
		explicit InternalActivationContext(const QUrl& pUrl);
		~InternalActivationContext() override = default;

		[[nodiscard]] QUrl getActivationURL() const override;
		bool sendProcessing() override;
		bool sendOperationAlreadyActive() override;
		bool sendErrorPage(http_status pStatusCode, const GlobalStatus& pStatus) override;
		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) override;
};

} // namespace governikus
