/*!
 * WebserviceActivationContext.h
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "ActivationContext.h"
#include "HttpRequest.h"

#include <QSharedPointer>

namespace governikus
{

class WebserviceActivationContext
	: public ActivationContext
{
	Q_OBJECT

	const QSharedPointer<HttpRequest> mRequest;

	void setCommonHeaders(HttpResponse& pResponse);

	public:
		WebserviceActivationContext(const QSharedPointer<HttpRequest>& pRequest);

		virtual ~WebserviceActivationContext();

		QUrl getActivationURL() const override;

		bool sendProcessing() override;

		bool sendOperationAlreadyActive() override;

		bool sendErrorPage(HttpStatusCode pStatusCode, const GlobalStatus& pStatus) override;

		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) override;
};

} /* namespace governikus */
