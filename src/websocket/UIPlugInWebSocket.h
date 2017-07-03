/*!
 * \brief UIPlugIn implementation of the Websocket.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "UIPlugInJsonApi.h"
#include "view/UIPlugIn.h"

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
		QWebSocketServer mServer;
		QScopedPointer<QWebSocket> mConnection;
		UIPlugInJsonApi* mJsonApi;

		static int WEBSOCKET_PORT;

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
		virtual ~UIPlugInWebSocket();

		static void setPort(int pPort);
		static int getPort();

		static const int WEBSOCKET_DEFAULT_PORT = 14727;
};

} /* namespace governikus */
