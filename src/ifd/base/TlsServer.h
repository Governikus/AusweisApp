/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include <QByteArray>
#include <QPointer>
#include <QSslConfiguration>
#include <QSslError>
#include <QSslPreSharedKeyAuthenticator>
#include <QSslSocket>
#include <QTcpServer>

#include <map>
#include <memory>


class test_IfdConnector;
class test_RemoteTlsServer;


namespace governikus
{

class TlsServer
	: public QTcpServer
{
	Q_OBJECT
	friend class ::test_IfdConnector;
	friend class ::test_RemoteTlsServer;

	private:
		std::map<QSslSocket*, std::unique_ptr<QSslSocket>> mPendingSockets;
		QPointer<QSslSocket> mSocket;
		QByteArray mPsk;

		bool releaseSocket(QSslSocket* pSocket);
		void incomingConnection(qintptr pSocketDescriptor) override;
		virtual QSslConfiguration sslConfiguration() const = 0;
		virtual bool acceptSslErrors(const QPointer<QSslSocket>& pSocket, const QList<QSslError>& pErrors) const = 0;
		virtual bool checkSslConfiguration(const QSslConfiguration& pSslConfiguration) const = 0;
		virtual void updateClientInfo(const QSslConfiguration& pSslConfiguration) = 0;

		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator, const QByteArray& pPsk) const;
		void onError(QSslSocket* pSocket, QAbstractSocket::SocketError pSocketError);
		void onSslErrors(QSslSocket* pSocket, const QList<QSslError>& pErrors) const;
		void onEncrypted(QSslSocket* pSocket);

	protected:
		[[nodiscard]] const QByteArray& getPsk() const;

	public:
		TlsServer();
		~TlsServer() override;
		void setPsk(const QByteArray& pPsk);
		void stopListening();
		virtual bool startListening(quint16 pPort) = 0;
		[[nodiscard]] bool hasPsk() const;
		[[nodiscard]] QSslCertificate getCurrentCertificate() const;

	Q_SIGNALS:
		void fireNewConnection(QTcpSocket* pSocket);
		void firePskChanged(const QByteArray& pPsk);
		void fireSocketError(QAbstractSocket::SocketError pSocketError);
};

} // namespace governikus
