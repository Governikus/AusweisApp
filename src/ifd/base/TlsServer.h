/*!
 * \brief QTcpServer with necessary TLS handling of remote device configuration.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QPointer>
#include <QSslConfiguration>
#include <QSslError>
#include <QSslPreSharedKeyAuthenticator>
#include <QSslSocket>
#include <QTcpServer>

namespace governikus
{

class TlsServer
	: public QTcpServer
{
	Q_OBJECT

	private:
		void incomingConnection(qintptr pSocketDescriptor) override;
		virtual QSslConfiguration sslConfiguration() const = 0;

	private Q_SLOTS:
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator);
		void onError(QAbstractSocket::SocketError pSocketError);
		virtual void onSslErrors(const QList<QSslError>& pErrors) = 0;
		virtual void onEncrypted() = 0;

	protected:
		QPointer<QSslSocket> mSocket;
		QByteArray mPsk;

	public:
		TlsServer();
		~TlsServer() override;
		void setPsk(const QByteArray& pPsk);
		void stopListening();
		virtual bool startListening(quint16 pPort) = 0;

	Q_SIGNALS:
		void fireNewConnection(QTcpSocket* pSocket);
		void firePskChanged(const QByteArray& pPsk);
};

} // namespace governikus
