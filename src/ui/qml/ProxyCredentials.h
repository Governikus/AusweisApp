/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAuthenticator>
#include <QEventLoop>
#include <QNetworkProxy>
#include <QtQml/qqmlregistration.h>


namespace governikus
{

class ProxyCredentials
	: public QObject
{
	Q_OBJECT
	QML_ELEMENT

	Q_PROPERTY(QString proposedUser READ getProposedUser CONSTANT)
	Q_PROPERTY(QString url READ getUrl CONSTANT)
	Q_PROPERTY(QString user READ getUser WRITE setUser NOTIFY fireProxyCredentialsChanged)
	Q_PROPERTY(QString password READ getPassword WRITE setPassword NOTIFY fireProxyCredentialsChanged)

	private:
		QEventLoop mLoop;
		QAuthenticator* mAuthenticator;
		const QString mProposedUser;
		const QString mUrl;

		[[nodiscard]] QString createUrl(const QNetworkProxy& pProxy) const;

	public:
		ProxyCredentials() = default;
		ProxyCredentials(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);

		[[nodiscard]] QString getProposedUser() const;
		[[nodiscard]] QString getUrl() const;

		[[nodiscard]] QString getUser() const;
		void setUser(const QString& pUser);

		[[nodiscard]] QString getPassword() const;
		void setPassword(const QString& pPassword);

		Q_INVOKABLE void confirmInput();
		void waitForConfirmation();

	Q_SIGNALS:
		void fireProxyCredentialsChanged();
};


} // namespace governikus
