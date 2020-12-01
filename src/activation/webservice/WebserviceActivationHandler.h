/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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

		/*!
		 * \brief Get the query items with lower-case keys, so we can support case-insensitive keys.
		 */
		static QMap<QString, QString> getQueryParameter(const QUrl& pUrl);

		void handleImageRequest(const QSharedPointer<HttpRequest>& pRequest, const QString& pImagePath) const;
		QByteArray guessImageContentType(const QString& pFileName) const;
		void handleShowUiRequest(UiModule pUiModule, const QSharedPointer<HttpRequest>& pRequest);
		void handleStatusRequest(StatusFormat pStatusFormat, const QSharedPointer<HttpRequest>& pRequest) const;

	private Q_SLOTS:
		void onNewRequest(const QSharedPointer<HttpRequest>& pRequest);

	public:
		WebserviceActivationHandler();
		virtual ~WebserviceActivationHandler() override;

		virtual bool start() override;
		virtual void stop() override;
};

} // namespace governikus
