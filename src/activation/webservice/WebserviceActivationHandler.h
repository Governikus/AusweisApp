/*!
 * WebserviceActivationHandler.h
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once


#include "ActivationHandler.h"
#include "qhttpserver/qhttprequest.h"
#include "qhttpserver/qhttpresponse.h"
#include "qhttpserver/qhttpserver.h"

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

	friend class ::test_WebserviceActivationHandler;

	QHttpServer mHttpServer;

	static void addStatusLine(QString& pContent, StatusFormat pStatusFormat, const QString& pKey, const QString& pValue);

	void handleImageRequest(QSharedPointer<QHttpResponse> pResponse, const QString& pImagePath);
	QString guessImageContentType(const QString& pFileName) const;
	void handleShowUiRequest(UiModule pUiModule, QSharedPointer<QHttpRequest> pRequest, QSharedPointer<QHttpResponse> pResponse);
	void handleStatusRequest(StatusFormat pStatusFormat, QSharedPointer<QHttpResponse> pResponse);

	private Q_SLOTS:
		void onNewRequest(QSharedPointer<QHttpRequest> pRequest, QSharedPointer<QHttpResponse> pResponse);

	public:
		static quint16 PORT;
		WebserviceActivationHandler();
		virtual ~WebserviceActivationHandler();

		quint16 getServerPort();
		virtual bool start() override;
		virtual void stop() override;

};

} /* namespace governikus */
