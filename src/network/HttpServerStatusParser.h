/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpServerRequestor.h"
#include "VersionInfo.h"

#include <QHostAddress>
#include <QNetworkReply>
#include <QSharedPointer>
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

		bool parseReply(const QSharedPointer<QNetworkReply>& pReply);

	public:
		HttpServerStatusParser(quint16 pPort, const QHostAddress& pHost);
		~HttpServerStatusParser() override = default;

		bool request();
		[[nodiscard]] const QString& getServerHeader() const;
		[[nodiscard]] const VersionInfo& getVersionInfo() const;
};

} // namespace governikus
