/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpRequest.h"
#include "context/ActivationContext.h"

#include <QSharedPointer>


namespace governikus
{

class WebserviceActivationContext
	: public ActivationContext
{
	Q_OBJECT

	private:
		const QSharedPointer<HttpRequest> mRequest;

		void setCommonHeaders(HttpResponse& pResponse) const;

	public:
		explicit WebserviceActivationContext(const QSharedPointer<HttpRequest>& pRequest);

		~WebserviceActivationContext() override = default;

		[[nodiscard]] QUrl getActivationURL() const override;

		bool sendProcessing() override;

		bool sendOperationAlreadyActive() override;

		bool sendErrorPage(http_status pStatusCode, const GlobalStatus& pStatus) override;

		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) override;
};

} // namespace governikus
