/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"
#include "HttpServer.h"

class test_WebserviceActivationHandler;

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

		static void addStatusLine(QString& pContent, StatusFormat pStatusFormat, const QString& pKey, const QString& pValue);

		void handleImageRequest(const QSharedPointer<HttpRequest>& pRequest, const QString& pImagePath);
		QByteArray guessImageContentType(const QString& pFileName) const;
		void handleShowUiRequest(UiModule pUiModule, const QSharedPointer<HttpRequest>& pRequest);
		void handleStatusRequest(StatusFormat pStatusFormat, const QSharedPointer<HttpRequest>& pRequest);

	private Q_SLOTS:
		void onNewRequest(const QSharedPointer<HttpRequest>& pRequest);

	public:
		WebserviceActivationHandler();
		virtual ~WebserviceActivationHandler() override;

		virtual bool start() override;
		virtual void stop() override;
};

} /* namespace governikus */
