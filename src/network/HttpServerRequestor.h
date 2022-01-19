/*
 * \brief Sends one time GET requests to server.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QEventLoop>
#include <QHostAddress>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QTimer>

namespace governikus
{

class HttpServerRequestor
	: public QObject
{
	Q_OBJECT

	private:
		QEventLoop mEventLoop;
		QTimer mTimer;

	public:
		HttpServerRequestor();
		~HttpServerRequestor() override;

		QSharedPointer<QNetworkReply> request(const QUrl& pUrl, int pTimeOut = 2000);
		static QUrl createUrl(const QString& pQuery, quint16 pPort, const QHostAddress& pHost = QHostAddress::LocalHost, const QString& pPath = QStringLiteral("/eID-Client"));

	private Q_SLOTS:
		void finished();
};

} // namespace governikus
