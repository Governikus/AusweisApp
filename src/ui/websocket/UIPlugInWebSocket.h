/*!
 * \brief UIPlugIn implementation of the Websocket.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpRequest.h"
#include "HttpServer.h"
#include "UIPlugIn.h"
#include "UIPlugInJsonApi.h"

#include <QDir>
#include <QMap>
#include <QScopedPointer>
#include <QWebSocket>
#include <QWebSocketServer>

namespace governikus
{

class UIPlugInWebSocket
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private:
		QSharedPointer<HttpServer> mHttpServer;
		QWebSocketServer mServer;
		QScopedPointer<QWebSocket, QScopedPointerDeleteLater> mConnection;
		QSharedPointer<HttpRequest> mRequest;
		UIPlugInJsonApi* mJsonApi;
		QSharedPointer<WorkflowContext> mContext;
		bool mUiDomination;

	private Q_SLOTS:
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted) override;
		virtual void onUiDominationReleased() override;
		void onNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest);
		void onNewConnection();
		void onClientDisconnected();
		void onTextMessageReceived(const QString& pMessage);
		void onJsonApiMessage(const QByteArray& pMessage);

	public:
		UIPlugInWebSocket();
		virtual ~UIPlugInWebSocket() override;
};

} // namespace governikus
