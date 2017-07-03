/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "UIPlugInWebSocket.h"

#include "view/UILoader.h"

#include <QCoreApplication>
#include <QFile>
#include <QLoggingCategory>
#include <QPluginLoader>


Q_DECLARE_LOGGING_CATEGORY(websocket)

using namespace governikus;


quint16 UIPlugInWebSocket::cWebSocketPort = UIPlugInWebSocket::WEBSOCKET_DEFAULT_PORT;


UIPlugInWebSocket::UIPlugInWebSocket()
	: UIPlugIn()
	, mServer(QCoreApplication::applicationName() + QLatin1Char('/') + QCoreApplication::applicationVersion(), QWebSocketServer::NonSecureMode)
	, mConnection(nullptr)
	, mJsonApi(nullptr)
	, mContext()
{
	if (!UILoader::getInstance().load(UIPlugInName::UIPlugInJsonApi))
	{
		qWarning(websocket) << "Cannot start WebSocket because JSON-API is missing";
		return;
	}

	mJsonApi = qobject_cast<UIPlugInJsonApi*>(UILoader::getInstance().getLoaded(UIPlugInName::UIPlugInJsonApi));
	Q_ASSERT(mJsonApi);
	connect(&mServer, &QWebSocketServer::newConnection, this, &UIPlugInWebSocket::onNewConnection);
	qDebug(websocket) << "Starting WebSocket..." << mServer.listen(QHostAddress::LocalHost, cWebSocketPort);
	if (!mServer.isListening())
	{
		qCritical(websocket) << mServer.errorString();
		return;
	}

	const quint16 port = mServer.serverPort();
	qDebug(websocket) << "Listening on port" << port;

#ifndef QT_NO_DEBUG
	if (cWebSocketPort == 0)
	{
		const QString path = WEBSOCKET_PORT_FILENAME(QCoreApplication::applicationPid());
		QFile file(path);
		if (file.open(QIODevice::WriteOnly))
		{
			QTextStream(&file) << port;
			qDebug(websocket) << "Stored port number to info file" << path;
		}
		else
		{
			qCritical(websocket) << "Failed to store port number to info file" << path;
		}
	}
#endif

}


UIPlugInWebSocket::~UIPlugInWebSocket()
{
}


void UIPlugInWebSocket::setPort(quint16 pPort)
{
	cWebSocketPort = pPort;
}


quint16 UIPlugInWebSocket::getPort()
{
	return cWebSocketPort;
}


void UIPlugInWebSocket::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	mContext = pContext;
}


void UIPlugInWebSocket::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext);

	mContext.clear();
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

	if (mContext)
	{
		const QSignalBlocker blocker(mJsonApi);
		Q_EMIT mContext->fireCancelWorkflow();
	}

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
