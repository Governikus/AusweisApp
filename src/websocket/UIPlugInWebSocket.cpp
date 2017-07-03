/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "UIPlugInWebSocket.h"

#include "view/UILoader.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QPluginLoader>


Q_DECLARE_LOGGING_CATEGORY(websocket)

using namespace governikus;


int UIPlugInWebSocket::WEBSOCKET_PORT = UIPlugInWebSocket::WEBSOCKET_DEFAULT_PORT;


UIPlugInWebSocket::UIPlugInWebSocket()
	: UIPlugIn()
	, mServer(QCoreApplication::applicationName() + QLatin1Char('/') + QCoreApplication::applicationVersion(), QWebSocketServer::NonSecureMode)
	, mConnection(nullptr)
	, mJsonApi(nullptr)
{
	if (UILoader::getInstance().load(UIPlugInName::UIPlugInJsonApi))
	{
		mJsonApi = qobject_cast<UIPlugInJsonApi*>(UILoader::getInstance().getLoaded(UIPlugInName::UIPlugInJsonApi));
		Q_ASSERT(mJsonApi);
		connect(&mServer, &QWebSocketServer::newConnection, this, &UIPlugInWebSocket::onNewConnection);
		qDebug(websocket) << "Starting WebSocket..." << mServer.listen(QHostAddress::LocalHost, WEBSOCKET_PORT);
		if (!mServer.isListening())
		{
			qCritical(websocket) << mServer.errorString();
		}
	}
	else
	{
		qWarning(websocket) << "Cannot start WebSocket because JSON-API is missing";
	}
}


UIPlugInWebSocket::~UIPlugInWebSocket()
{
}


void UIPlugInWebSocket::setPort(int pPort)
{
	WEBSOCKET_PORT = pPort;
}


int UIPlugInWebSocket::getPort()
{
	return WEBSOCKET_PORT;
}


void UIPlugInWebSocket::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
}


void UIPlugInWebSocket::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
}


void UIPlugInWebSocket::onNewConnection()
{
	if (mServer.hasPendingConnections())
	{
		auto connection = mServer.nextPendingConnection();
		if (mConnection)
		{
			qDebug(websocket) << "Client is already connected...";
			connection->close(QWebSocketProtocol::CloseCodePolicyViolated);
			return;
		}
		mConnection.reset(connection);

		connect(mConnection.data(), &QWebSocket::disconnected, this, &UIPlugInWebSocket::onClientDisconnected);
		connect(mConnection.data(), &QWebSocket::textMessageReceived, this, &UIPlugInWebSocket::onTextMessageReceived);
		connect(mJsonApi, &UIPlugInJsonApi::fireMessage, this, &UIPlugInWebSocket::onJsonApiMessage);
	}
}


void UIPlugInWebSocket::onClientDisconnected()
{
	qDebug(websocket) << "Client disconnected...";
	mConnection.reset();
	disconnect(mJsonApi, &UIPlugInJsonApi::fireMessage, this, &UIPlugInWebSocket::onJsonApiMessage);
}


void UIPlugInWebSocket::onTextMessageReceived(const QString& pMessage)
{
	if (mConnection)
	{
		mJsonApi->doMessageProcessing(pMessage.toUtf8());
	}
}


void UIPlugInWebSocket::onJsonApiMessage(const QByteArray& pMessage)
{
	if (mConnection)
	{
		mConnection->sendTextMessage(QString::fromUtf8(pMessage));
	}
}


void UIPlugInWebSocket::doShutdown()
{
	if (mConnection)
	{
		mConnection->close(QWebSocketProtocol::CloseCodeGoingAway);
	}
}
