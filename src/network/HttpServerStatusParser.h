/*!
 * HttpServerStatusParser.h
 *
 * \brief Sends a status request to given url:port and tries to detect details and server header.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"
#include "HttpServerRequestor.h"
#include "VersionInfo.h"
#include <QHostAddress>
#include <QNetworkReply>
#include <QUrl>

namespace governikus
{

class HttpServerStatusParser
	: public QObject
{
	Q_OBJECT

	private:
		QUrl mUrl;
		QString mServerHeader;
		VersionInfo mVersionInfo;
		HttpServerRequestor mRequestor;

		bool parseReply(const QPointer<QNetworkReply>& pReply);

	public:
		HttpServerStatusParser(int pPort, const QHostAddress& pHost = QHostAddress::LocalHost);
		virtual ~HttpServerStatusParser();

		bool request();
		const QString& getServerHeader() const;
		const VersionInfo& getVersionInfo() const;
};

} /* namespace governikus */
