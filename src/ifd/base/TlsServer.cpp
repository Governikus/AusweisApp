/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "TlsServer.h"

#include <QLoggingCategory>
#include <QNetworkProxy>

Q_DECLARE_LOGGING_CATEGORY(ifd)

using namespace governikus;


TlsServer::TlsServer()
	: QTcpServer()
	, mSocket()
	, mPsk()
{
	//listening with proxy leads to socket error QNativeSocketEnginePrivate::InvalidProxyTypeString
	setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
}


TlsServer::~TlsServer()
{
	if (mSocket)
	{
		mSocket->deleteLater();
	}
}


void TlsServer::setPsk(const QByteArray& pPsk)
{
	if (pPsk != mPsk)
	{
		mPsk = pPsk;
		Q_EMIT firePskChanged(pPsk);
	}
}


void TlsServer::stopListening()
{
	close();
	mPsk.clear();
}


void TlsServer::incomingConnection(qintptr pSocketDescriptor)
{
	if (mSocket.isNull())
	{
		mSocket = new QSslSocket();

		QSslConfiguration config = sslConfiguration();
		mSocket->setSslConfiguration(config);

		if (Q_LIKELY(mSocket->setSocketDescriptor(pSocketDescriptor)))
		{
			connect(mSocket.data(), &QAbstractSocket::errorOccurred, this, &TlsServer::onError);
			connect(mSocket.data(), QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
					this, &TlsServer::onSslErrors);
			connect(mSocket.data(), &QSslSocket::preSharedKeyAuthenticationRequired,
					this, &TlsServer::onPreSharedKeyAuthenticationRequired);
			connect(mSocket.data(), &QSslSocket::encrypted, this, &TlsServer::onEncrypted);

			qCDebug(ifd).noquote() << "Starting encryption for incoming connection from" << mSocket->peerAddress().toString();
			mSocket->startServerEncryption();
		}
		else
		{
			qCDebug(ifd) << "Failed to set the socket descriptor";
			delete mSocket.data();
		}
	}
	else
	{
		QTcpSocket socket;
		socket.setSocketDescriptor(pSocketDescriptor);
		qCDebug(ifd).noquote() << "Socket already connected. Incoming connection from" << socket.peerAddress().toString() << "refused";
		socket.abort();
	}
}


void TlsServer::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator)
{
	qCDebug(ifd) << "Client requests PSK authentication | identity:" << pAuthenticator->identity() << "| hint:" << pAuthenticator->identityHint();
	pAuthenticator->setPreSharedKey(mPsk);
}


void TlsServer::onError(QAbstractSocket::SocketError pSocketError)
{
	qCDebug(ifd) << "Socket error:" << pSocketError << mSocket->errorString();
	mSocket->deleteLater();
}
