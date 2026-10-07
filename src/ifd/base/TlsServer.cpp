/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "TlsServer.h"

#include "TlsChecker.h"

#include <QLoggingCategory>
#include <QNetworkProxy>
#include <QSslCipher>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


bool TlsServer::releaseSocket(QSslSocket* pSocket)
{
	const auto socket = mPendingSockets.extract(pSocket);
	if (socket.empty())
	{
		qCWarning(ifd) << "Not able to find requested socket";
		return false;
	}

	if (socket.mapped().release() != pSocket)
	{
		qCCritical(ifd) << "Returned socket does not match requested socket";
		Q_ASSERT(false);
		return false;
	}

	return true;
}


void TlsServer::incomingConnection(qintptr pSocketDescriptor)
{
	if (mSocket)
	{
		QTcpSocket socket;
		socket.setSocketDescriptor(pSocketDescriptor);
		qCDebug(ifd).noquote() << "Client (" << socket.peerAddress().toString() << "): Socket already connected, incoming connection refused";
		socket.abort();

		return;
	}

	auto socket = std::make_unique<QSslSocket>();
	socket->setSslConfiguration(sslConfiguration());
	if (Q_UNLIKELY(!socket->setSocketDescriptor(pSocketDescriptor)))
	{
		qCDebug(ifd) << "Failed to set the socket descriptor";

		return;
	}

	connect(socket.get(), &QAbstractSocket::errorOccurred, this,
			[this, socket = socket.get()](QAbstractSocket::SocketError pSocketError){
				onError(socket, pSocketError);
			});
	connect(socket.get(), &QSslSocket::alertReceived, this, [](QSsl::AlertLevel pLevel, QSsl::AlertType pType, const QString& pDesc)
			{
				qCInfo(ifd) << "Got TLS alert:" << pLevel << pType << pDesc;
			});
	connect(socket.get(), &QSslSocket::alertSent, this, [](QSsl::AlertLevel pLevel, QSsl::AlertType pType, const QString& pDesc)
			{
				qCInfo(ifd) << "Sent TLS alert:" << pLevel << pType << pDesc;
			});
	connect(socket.get(), QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this,
			[this, socket = socket.get()](const QList<QSslError>& pErrors){
				onSslErrors(socket, pErrors);
			});
	connect(socket.get(), &QSslSocket::preSharedKeyAuthenticationRequired, this,
			[this, psk = mPsk](QSslPreSharedKeyAuthenticator* pAuthenticator){
				onPreSharedKeyAuthenticationRequired(pAuthenticator, psk);
			});
	connect(socket.get(), &QSslSocket::encrypted, this,
			[this, socket = socket.get()](){
				onEncrypted(socket);
			});

	qCDebug(ifd).noquote() << "Client (" << socket->peerAddress().toString() << "): Starting encryption for incoming connection";
	socket->startServerEncryption();
	mPendingSockets[socket.get()] = std::move(socket);
}


void TlsServer::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator, const QByteArray& pPsk) const
{
	qCDebug(ifd) << "Client requests PSK authentication | identity:" << pAuthenticator->identity() << "| hint:" << pAuthenticator->identityHint();
	pAuthenticator->setPreSharedKey(pPsk);
}


void TlsServer::onError(QSslSocket* pSocket, QAbstractSocket::SocketError pSocketError)
{
	if (!releaseSocket(pSocket))
	{
		return;
	}

	qCDebug(ifd).noquote() << "Client (" << pSocket->peerAddress().toString() << ") socket error:" << pSocketError << "|" << pSocket->errorString();
	pSocket->disconnect(this);
	pSocket->deleteLater();

	Q_EMIT fireSocketError(pSocketError);
}


void TlsServer::onSslErrors(QSslSocket* pSocket, const QList<QSslError>& pErrors) const
{
	if (!mPendingSockets.contains(pSocket))
	{
		qCWarning(ifd) << "Got unknown socket";
		return;
	}

	if (acceptSslErrors(pSocket, pErrors))
	{
		return;
	}

	qCDebug(ifd).noquote() << "Client (" << pSocket->peerAddress().toString() << ") is not allowed | cipher:" << pSocket->sessionCipher() << "| certificate:" << pSocket->peerCertificate() << "| error:" << pErrors;
}


void TlsServer::onEncrypted(QSslSocket* pSocket)
{
	if (!releaseSocket(pSocket))
	{
		return;
	}

	qCDebug(ifd).noquote() << "Client (" << pSocket->peerAddress().toString() << ") successfully finished encryption";
	pSocket->disconnect(this);

	const auto& cfg = pSocket->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(ifd));
	if (!checkSslConfiguration(cfg))
	{
		pSocket->abort();
		pSocket->deleteLater();
		return;
	}

	qCDebug(ifd).noquote() << "Client (" << pSocket->peerAddress().toString() << ") accepted";
	mSocket = pSocket;
	updateClientInfo(cfg);

	for (auto it = mPendingSockets.begin(); it != mPendingSockets.end(); it = mPendingSockets.erase(it))
	{
		it->second->disconnect(this);
		it->second->close();
	}

	Q_EMIT fireNewConnection(pSocket);
}


const QByteArray& TlsServer::getPsk() const
{
	return mPsk;
}


TlsServer::TlsServer()
	: QTcpServer()
	, mPendingSockets()
	, mSocket()
	, mPsk()
{
	//listening with proxy leads to socket error QNativeSocketEnginePrivate::InvalidProxyTypeString
	setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
}


TlsServer::~TlsServer() = default;


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


bool TlsServer::hasPsk() const
{
	return !mPsk.isEmpty();
}


QSslCertificate TlsServer::getCurrentCertificate() const
{
	return mSocket ? mSocket->sslConfiguration().peerCertificate() : QSslCertificate();
}
