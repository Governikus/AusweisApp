/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationContext.h"
#include "HttpRequest.h"

#include <QSharedPointer>

class test_WebserviceActivationContext;

namespace governikus
{

class WebserviceActivationContext
	: public ActivationContext
{
	Q_OBJECT
	friend class ::test_WebserviceActivationContext;

	const QSharedPointer<HttpRequest> mRequest;

	void setCommonHeaders(HttpResponse& pResponse);

	public:
		WebserviceActivationContext(const QSharedPointer<HttpRequest>& pRequest);

		virtual ~WebserviceActivationContext() override = default;

		QUrl getActivationURL() const override;

		bool sendProcessing() override;

		bool sendOperationAlreadyActive() override;

		bool sendErrorPage(http_status pStatusCode, const GlobalStatus& pStatus) override;

		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) override;
};

} // namespace governikus
