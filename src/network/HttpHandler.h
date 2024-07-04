/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "HttpRequest.h"

#include <QCoreApplication>

class test_UiPluginWebService;

namespace governikus
{

defineEnumType(StatusFormat,
		PLAIN,
		JSON
		)

class HttpHandler
{
	Q_DECLARE_TR_FUNCTIONS(governikus::HttpHandler)
	friend class ::test_UiPluginWebService;

	private:
		[[nodiscard]] QByteArray guessImageContentType(const QString& pFileName) const;
		void setCorsResponseHeaders(HttpResponse& pRequest) const;
		void handleCorsRequest(const QSharedPointer<HttpRequest>& pRequest) const;
		bool handleGetRequest(const QSharedPointer<HttpRequest>& pRequest, const QUrl& pUrl);

	protected:
		virtual ~HttpHandler() = default;

		void handle(const QSharedPointer<HttpRequest>& pRequest);

		virtual void handleImageRequest(const QSharedPointer<HttpRequest>& pRequest, const QString& pImagePath) const;
		virtual void handleStatusRequest(StatusFormat pStatusFormat, const QSharedPointer<HttpRequest>& pRequest) const;
		virtual void handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest) = 0;
		virtual void handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest) = 0;
};

} // namespace governikus
