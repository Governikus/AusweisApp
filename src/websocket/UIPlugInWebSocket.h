/*!
 * \brief UIPlugIn implementation of the Websocket.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UIPlugInJsonApi.h"
#include "view/UIPlugIn.h"

#include <QDir>
#include <QScopedPointer>
#include <QWebSocket>
#include <QWebSocketServer>

namespace governikus
{

#define WEBSOCKET_PORT_FILENAME(PID) (QDir::tempPath() + QDir::separator() + QStringLiteral("web_socket_port-") + QString::number(PID))

class UIPlugInWebSocket
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private:
		QWebSocketServer mServer;
		QScopedPointer<QWebSocket> mConnection;
		UIPlugInJsonApi* mJsonApi;
		QSharedPointer<WorkflowContext> mContext;

		static quint16 cWebSocketPort;

	private Q_SLOTS:
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onNewConnection();
		void onClientDisconnected();
		void onTextMessageReceived(const QString& pMessage);

		void onJsonApiMessage(const QByteArray& pMessage);

	public:
		UIPlugInWebSocket();
		virtual ~UIPlugInWebSocket() override;

		static void setPort(quint16 pPort);
		static quint16 getPort();

		static const quint16 WEBSOCKET_DEFAULT_PORT = 14727;
};

} /* namespace governikus */
