/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteConnectorImpl.h"

#include "AppSettings.h"
#include "Env.h"
#include "LogHandler.h"
#include "RemoteDispatcher.h"
#include "SecureStorage.h"
#include "TlsChecker.h"
#include "WebSocketChannel.h"

#include <QLoggingCategory>
#include <QMutableVectorIterator>
#include <QSslPreSharedKeyAuthenticator>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(remote_device)


namespace governikus
{
template<> RemoteConnector* createNewObject<RemoteConnector*>()
{
	return new RemoteConnectorImpl;
}


class ConnectRequest
	: public QObject
{
	Q_OBJECT

	private:
		const RemoteDeviceDescriptor mRemoteDeviceDescriptor;
		const QByteArray mPsk;
		const QSharedPointer<QWebSocket> mSocket;
		QTimer mTimer;

	private Q_SLOTS:
		void onConnected();
		void onError(QAbstractSocket::SocketError pError);
		void onTimeout();
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator);
		void onSslErrors(const QList<QSslError>& pErrors);

	public:
		ConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor,
				const QByteArray& pPsk,
				int pTimeoutMs);
		virtual ~ConnectRequest() = default;

		const RemoteDeviceDescriptor& getRemoteDeviceDescriptor() const;

		void start();

	Q_SIGNALS:
		void fireConnectionCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor,
				const QSharedPointer<QWebSocket>& pWebSocket);
		void fireConnectionError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const RemoteErrorCode& pError);
		void fireConnectionTimeout(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);
};

} // namespace governikus


using namespace governikus;


void ConnectRequest::onConnected()
{
	mTimer.stop();

	const auto& cfg = mSocket->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(remote_device));

	if (!TlsChecker::hasValidCertificateKeyLength(cfg.peerCertificate()) ||
			(!cfg.ephemeralServerKey().isNull() && !TlsChecker::hasValidEphemeralKeyLength(cfg.ephemeralServerKey())))
	{
		qCCritical(remote_device) << "Server denied... abort connection!";
		mSocket->abort();
		Q_EMIT fireConnectionError(mRemoteDeviceDescriptor, RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
		return;
	}

	qCDebug(remote_device) << "Connected to remote device";

	auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& pairingCiphers = SecureStorage::getInstance().getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getCiphers();
	if (pairingCiphers.contains(cfg.sessionCipher()))
	{
		qCDebug(remote_device) << "Pairing completed | Add certificate:" << cfg.peerCertificate();
		settings.addTrustedCertificate(cfg.peerCertificate());
		settings.save();
	}
	else
	{
		auto info = settings.getRemoteInfo(cfg.peerCertificate());
		info.setLastConnected(QDateTime::currentDateTime());
		settings.updateRemoteInfo(info);
	}

	Q_EMIT fireConnectionCreated(mRemoteDeviceDescriptor, mSocket);
}


void ConnectRequest::onError(QAbstractSocket::SocketError pError)
{
	qCWarning(remote_device) << "Connection error:" << pError;

	mTimer.stop();
	if (pError == QAbstractSocket::SocketError::RemoteHostClosedError)
	{
		Q_EMIT fireConnectionError(mRemoteDeviceDescriptor, RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
	}
	else
	{
		Q_EMIT fireConnectionError(mRemoteDeviceDescriptor, RemoteErrorCode::CONNECTION_ERROR);
	}
}


void ConnectRequest::onTimeout()
{
	Q_EMIT fireConnectionTimeout(mRemoteDeviceDescriptor);
}


void ConnectRequest::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator)
{
	qCDebug(remote_device) << "Request pairing...";
	pAuthenticator->setPreSharedKey(mPsk);
}


void ConnectRequest::onSslErrors(const QList<QSslError>& pErrors)
{
	QList<QSslError::SslError> allowedErrors = {
		QSslError::HostNameMismatch
	};

	const auto& config = mSocket->sslConfiguration();
	const auto& pairingCiphers = SecureStorage::getInstance().getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getCiphers();
	if (pairingCiphers.contains(config.sessionCipher()))
	{
		allowedErrors << QSslError::SelfSignedCertificate;
	}

	bool ignoreErrors = true;
	for (const auto& error : pErrors)
	{
		if (!allowedErrors.contains(error.error()))
		{
			ignoreErrors = false;
			break;
		}
	}

	if (ignoreErrors)
	{
		mSocket->ignoreSslErrors();
		return;
	}

	qCDebug(remote_device) << "Server is untrusted | cipher:" << config.sessionCipher() << "| certificate:" << config.peerCertificate() << "| error:" << pErrors;
}


ConnectRequest::ConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor,
		const QByteArray& pPsk,
		int pTimeoutMs)
	: mRemoteDeviceDescriptor(pRemoteDeviceDescriptor)
	, mPsk(pPsk)
	, mSocket(new QWebSocket(), &QObject::deleteLater)
	, mTimer()
{
	auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const bool invalidLength = !TlsChecker::hasValidCertificateKeyLength(remoteServiceSettings.getCertificate());
	if (!remoteServiceSettings.checkAndGenerateKey(invalidLength))
	{
		qCCritical(remote_device) << "Cannot get required key/certificate for tls";
		return;
	}

	QSslConfiguration config;
	if (mPsk.isEmpty())
	{
		config = SecureStorage::getInstance().getTlsConfigRemote().getConfiguration();
		config.setCaCertificates(remoteServiceSettings.getTrustedCertificates());
		qCCritical(remote_device) << "Start reconnect to server";
	}
	else
	{
		config = SecureStorage::getInstance().getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getConfiguration();
		qCCritical(remote_device) << "Start pairing to server";
	}
	config.setPrivateKey(remoteServiceSettings.getKey());
	config.setLocalCertificate(remoteServiceSettings.getCertificate());
	config.setPeerVerifyMode(QSslSocket::VerifyPeer);
	mSocket->setSslConfiguration(config);

	connect(mSocket.data(), &QWebSocket::preSharedKeyAuthenticationRequired, this, &ConnectRequest::onPreSharedKeyAuthenticationRequired);
	connect(mSocket.data(), &QWebSocket::sslErrors, this, &ConnectRequest::onSslErrors);

#ifndef QT_NO_NETWORKPROXY
	mSocket->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
#endif
	connect(mSocket.data(), &QWebSocket::connected, this, &ConnectRequest::onConnected);
	connect(mSocket.data(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ConnectRequest::onError);

	mTimer.setSingleShot(true);
	mTimer.setInterval(pTimeoutMs);
	connect(&mTimer, &QTimer::timeout, this, &ConnectRequest::onTimeout);
}


const RemoteDeviceDescriptor& ConnectRequest::getRemoteDeviceDescriptor() const
{
	return mRemoteDeviceDescriptor;
}


void ConnectRequest::start()
{
	mSocket->open(mRemoteDeviceDescriptor.getUrl());
	mTimer.start();
}


void RemoteConnectorImpl::removeRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
{
	QMutableVectorIterator<QSharedPointer<ConnectRequest> > requestIterator(mPendingRequests);
	while (requestIterator.hasNext())
	{
		const QSharedPointer<ConnectRequest> item = requestIterator.next();
		Q_ASSERT(item);
		if (item.isNull())
		{
			qCCritical(remote_device) << "Unexpected null pending request";

			requestIterator.remove();
		}
		else if (item->getRemoteDeviceDescriptor() == pRemoteDeviceDescriptor)
		{
			requestIterator.remove();
		}
	}
}


void RemoteConnectorImpl::onConnectionCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor,
		const QSharedPointer<QWebSocket>& pWebSocket)
{
	const QSharedPointer<DataChannel> channel(new WebSocketChannel(pWebSocket), &QObject::deleteLater);
	const IfdVersion::Version latestSupportedVersion = IfdVersion::selectLatestSupported(pRemoteDeviceDescriptor.getApiVersions());
	const QSharedPointer<RemoteDispatcherClient> dispatcher(Env::create<RemoteDispatcherClient*>(latestSupportedVersion, channel), &QObject::deleteLater);

	removeRequest(pRemoteDeviceDescriptor);

	Q_EMIT fireRemoteDispatcherCreated(pRemoteDeviceDescriptor, dispatcher);
}


void RemoteConnectorImpl::onConnectionError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const RemoteErrorCode& pError)
{
	removeRequest(pRemoteDeviceDescriptor);

	Q_EMIT fireRemoteDispatcherError(pRemoteDeviceDescriptor, pError);
}


void RemoteConnectorImpl::onConnectionTimeout(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
{
	removeRequest(pRemoteDeviceDescriptor);

	Q_EMIT fireRemoteDispatcherError(pRemoteDeviceDescriptor, RemoteErrorCode::CONNECTION_TIMEOUT);
}


RemoteConnectorImpl::RemoteConnectorImpl(int pConnectTimeoutMs)
	: mConnectTimeoutMs(pConnectTimeoutMs)
	, mPendingRequests()
{
}


void RemoteConnectorImpl::onConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QString& pPsk)
{
	if (pRemoteDeviceDescriptor.isNull() || pRemoteDeviceDescriptor.getIfdName().isEmpty())
	{
		Q_EMIT fireRemoteDispatcherError(pRemoteDeviceDescriptor, RemoteErrorCode::INVALID_REQUEST);
		return;
	}

	// Currently, we only support API level 1.
	if (!pRemoteDeviceDescriptor.isSupported())
	{
		Q_EMIT fireRemoteDispatcherError(pRemoteDeviceDescriptor, RemoteErrorCode::NO_SUPPORTED_API_LEVEL);
		return;
	}

	const QSharedPointer<ConnectRequest> newRequest(new ConnectRequest(pRemoteDeviceDescriptor, pPsk.toUtf8(), mConnectTimeoutMs), &QObject::deleteLater);
	mPendingRequests += newRequest;
	connect(newRequest.data(), &ConnectRequest::fireConnectionCreated, this, &RemoteConnectorImpl::onConnectionCreated);
	connect(newRequest.data(), &ConnectRequest::fireConnectionError, this, &RemoteConnectorImpl::onConnectionError);
	connect(newRequest.data(), &ConnectRequest::fireConnectionTimeout, this, &RemoteConnectorImpl::onConnectionTimeout);
	newRequest->start();
}


#include "RemoteConnectorImpl.moc"
