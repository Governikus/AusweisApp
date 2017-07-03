/*!
 * WebserviceActivationContext.h
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "ActivationContext.h"
#include "qhttpserver/qhttprequest.h"
#include "qhttpserver/qhttpresponse.h"

#include <QSharedPointer>


class QHttpResponse;


namespace governikus
{

class WebserviceActivationContext
	: public ActivationContext
{
	Q_OBJECT

	const QSharedPointer<QHttpRequest> mRequest;
	const QSharedPointer<QHttpResponse> mResponse;

	void setCommonHeaders(QSharedPointer<QHttpResponse> pResponse);

	public:
		WebserviceActivationContext(QSharedPointer<QHttpRequest> pRequest, QSharedPointer<QHttpResponse> pResponse);

		virtual ~WebserviceActivationContext();

		QUrl getActivationURL() override;

		bool sendProcessing() override;

		bool sendOperationAlreadyActive() override;

		bool sendErrorPage(HttpStatusCode pStatusCode, const Result& pResult) override;

		bool sendRedirect(const QUrl& pRedirectAddress, const Result& pResult) override;

		static void setServerHeader(QHttpResponse& pReply);
};

} /* namespace governikus */
