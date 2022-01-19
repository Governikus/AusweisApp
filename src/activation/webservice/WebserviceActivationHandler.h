/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"
#include "HttpServer.h"


namespace governikus
{

/*!
 * This ActivationHandler implements an API through a local web service,
 * as specified by TR-03124-1.
 */
class WebserviceActivationHandler
	: public ActivationHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ActivationHandler" FILE "metadata.json")
	Q_INTERFACES(governikus::ActivationHandler)

	private:
		friend class ::test_WebserviceActivationHandler;
		QSharedPointer<HttpServer> mServer;

		void handleImageRequest(const QSharedPointer<HttpRequest>& pRequest, const QString& pImagePath) const;
		[[nodiscard]] QByteArray guessImageContentType(const QString& pFileName) const;
		void handleShowUiRequest(UiModule pUiModule, const QSharedPointer<HttpRequest>& pRequest);
		void handleStatusRequest(StatusFormat pStatusFormat, const QSharedPointer<HttpRequest>& pRequest) const;

	private Q_SLOTS:
		void onNewRequest(const QSharedPointer<HttpRequest>& pRequest);

	public:
		WebserviceActivationHandler();
		~WebserviceActivationHandler() override;

		bool start() override;
		void stop() override;
};

} // namespace governikus
