/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "WebSocketChannel.h"

#include "RemoteServiceSettings.h"

#include <QLoggingCategory>
#include <QThread>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;

namespace
{
const int PING_PONG_TIMEOUT_MS = 5000;
} // namespace


QString WebSocketChannel::makeConnectionId(const QSharedPointer<QWebSocket>& pConnection)
{
	if (pConnection)
	{
		return RemoteServiceSettings::generateFingerprint(pConnection->sslConfiguration().peerCertificate());
	}

	return QString();
}


WebSocketChannel::WebSocketChannel(const QSharedPointer<QWebSocket>& pConnection)
	: mConnection(pConnection)
	, mId(makeConnectionId(pConnection))
	, mPingTimer()
	, mPongTimer()
{
	if (mConnection)
	{
		connect(mConnection.data(), &QWebSocket::textMessageReceived, this, &WebSocketChannel::onReceived);
		connect(mConnection.data(), &QWebSocket::disconnected, this, &WebSocketChannel::onDisconnected);
		connect(&mPingTimer, &QTimer::timeout, this, &WebSocketChannel::onPingScheduled);
		connect(mConnection.data(), &QWebSocket::pong, this, &WebSocketChannel::onPongReceived);
		connect(&mPongTimer, &QTimer::timeout, this, &WebSocketChannel::onPongTimeout);

		mPingTimer.setInterval(PING_PONG_TIMEOUT_MS);
		mPingTimer.setSingleShot(true);
		mPongTimer.setInterval(PING_PONG_TIMEOUT_MS);
		mPongTimer.setSingleShot(true);
		mPingTimer.start();
	}
}


WebSocketChannel::~WebSocketChannel()
{
	if (mConnection)
	{
		disconnect(mConnection.data(), &QWebSocket::textMessageReceived, this, &WebSocketChannel::onReceived);
		disconnect(mConnection.data(), &QWebSocket::disconnected, this, &WebSocketChannel::onDisconnected);
		disconnect(mConnection.data(), &QWebSocket::pong, this, &WebSocketChannel::onPongReceived);
		disconnect(&mPingTimer, &QTimer::timeout, this, &WebSocketChannel::onPingScheduled);
		disconnect(&mPongTimer, &QTimer::timeout, this, &WebSocketChannel::onPongTimeout);
	}
}


void WebSocketChannel::send(const QByteArray& pDataBlock)
{
	if (mConnection)
	{
		mConnection->sendTextMessage(QString::fromUtf8(pDataBlock));
	}
}


void WebSocketChannel::close()
{
	if (mConnection)
	{
		// QWebSocket.close() must be executed in the thread in which the QWebSocket object lives.
		if (mConnection->thread() == QThread::currentThread())
		{
			mConnection->close();
		}
		else
		{
			qCWarning(ifd) << "Tried to close web socket from the wrong thread: skipping";
		}
	}
}


const QString& WebSocketChannel::getId() const
{
	return mId;
}


void WebSocketChannel::onReceived(const QString& pMessage)
{
	Q_EMIT fireReceived(pMessage.toUtf8());
}


void WebSocketChannel::onDisconnected()
{
	mPingTimer.stop();
	mPongTimer.stop();

	Q_ASSERT(mConnection);
	if (mConnection)
	{
		Q_EMIT fireClosed(mConnection->closeCode() == QWebSocketProtocol::CloseCodeNormal ?
				GlobalStatus::Code::No_Error :
				GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);
	}
}


void WebSocketChannel::onPingScheduled()
{
	mConnection->ping();
	mPongTimer.start();
}


void WebSocketChannel::onPongReceived()
{
	mPongTimer.stop();
	mPingTimer.start();
}


void WebSocketChannel::onPongTimeout()
{
	qCDebug(ifd) << "No pong received from remote for" << (PING_PONG_TIMEOUT_MS / 1000) << "seconds, closing socket";
	close();
}
