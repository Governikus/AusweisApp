/*!
 * \brief QTcpServer with necessary TLS handling of remote device configuration.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QPointer>
#include <QSslError>
#include <QSslPreSharedKeyAuthenticator>
#include <QSslSocket>
#include <QTcpServer>

namespace governikus
{

class RemoteTlsServer
	: public QTcpServer
{
	Q_OBJECT

	private:
		QPointer<QSslSocket> mSocket;
		QByteArray mPsk;
		virtual void incomingConnection(qintptr pSocketDescriptor) override;

	private Q_SLOTS:
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator);
		void onError(QAbstractSocket::SocketError pSocketError);
		void onSslErrors(const QList<QSslError>& pErrors);
		void onEncrypted();

	public:
		RemoteTlsServer();
		bool listen();
		void setPairing(bool pEnable = true);
		QSslCertificate getCurrentCertificate() const;

	Q_SIGNALS:
		void newConnection(QTcpSocket* pSocket);
		void firePskChanged(const QByteArray& pPsk);
};

} // namespace governikus
