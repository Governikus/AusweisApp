/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "HttpRequest.h"

class test_UIPlugInWebService;

namespace governikus
{

defineEnumType(StatusFormat,
		PLAIN,
		JSON
		)

class HttpHandler
{
	Q_DECLARE_TR_FUNCTIONS(governikus::HttpHandler)
	friend class ::test_UIPlugInWebService;

	private:
		[[nodiscard]] QByteArray guessImageContentType(const QString& pFileName) const;

	protected:
		virtual ~HttpHandler() = default;

		void handle(const QSharedPointer<HttpRequest>& pRequest);

		virtual void handleImageRequest(const QSharedPointer<HttpRequest>& pRequest, const QString& pImagePath) const;
		virtual void handleStatusRequest(StatusFormat pStatusFormat, const QSharedPointer<HttpRequest>& pRequest) const;
		virtual void handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest) = 0;
		virtual void handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest) = 0;


};

} // namespace governikus
