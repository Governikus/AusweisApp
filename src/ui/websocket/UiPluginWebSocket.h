/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UiPlugin implementation of the Websocket.
 */

#pragma once

#include "HttpRequest.h"
#include "HttpServer.h"
#include "UiPlugin.h"
#include "UiPluginJson.h"

#include <QDir>
#include <QMap>
#include <QScopedPointer>
#include <QWebSocket>
#include <QWebSocketServer>

namespace governikus
{

class UiPluginWebSocket
	: public UiPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)

	private:
		QSharedPointer<HttpServer> mHttpServer;
		QWebSocketServer mServer;
		QScopedPointer<QWebSocket, QScopedPointerDeleteLater> mConnection;
		QSharedPointer<HttpRequest> mRequest;
		QPointer<UiPluginJson> mJson;
		QSharedPointer<WorkflowContext> mContext;
		bool mUiDomination;
		bool mUiDominationPrevUsedAsSDK;

	private Q_SLOTS:
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest);
		void onNewConnection();
		void onClientDisconnected();
		void onTextMessageReceived(const QString& pMessage);
		void onJsonMessage(const QByteArray& pMessage);

	public:
		UiPluginWebSocket();
		~UiPluginWebSocket() override = default;

		[[nodiscard]] bool initialize() override;
};

} // namespace governikus
