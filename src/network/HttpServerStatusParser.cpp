/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "HttpServerStatusParser.h"

#include "LogHandler.h"
#include "NetworkManager.h"

#include <http_parser.h>
#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


HttpServerStatusParser::HttpServerStatusParser(quint16 pPort, const QHostAddress& pHost)
	: QObject()
	, mUrl(HttpServerRequestor::createUrl(QStringLiteral("Status"), pPort, pHost))
	, mServerHeader()
	, mVersionInfo()
	, mRequestor()
{
}


HttpServerStatusParser::~HttpServerStatusParser()
{
}


bool HttpServerStatusParser::request()
{
	// if we were called multiple time we need to clear() here
	mVersionInfo = VersionInfo();
	mServerHeader.clear();

	const auto reply = mRequestor.request(mUrl);
	if (!reply.isNull())
	{
		return parseReply(reply);
	}

	return false;
}


const QString& HttpServerStatusParser::getServerHeader() const
{
	return mServerHeader;
}


const VersionInfo& HttpServerStatusParser::getVersionInfo() const
{
	return mVersionInfo;
}


bool HttpServerStatusParser::parseReply(const QSharedPointer<QNetworkReply>& pReply)
{
	mServerHeader = pReply->header(QNetworkRequest::KnownHeaders::ServerHeader).toString();
	if (mServerHeader.isEmpty())
	{
		mServerHeader = pReply->header(QNetworkRequest::KnownHeaders::UserAgentHeader).toString();
		if (!mServerHeader.isEmpty())
		{
			qCDebug(network) << "Server header is undefined... using User-Agent";
		}
	}

	const auto statusCode = NetworkManager::getLoggedStatusCode(pReply, spawnMessageLogger(network));
	if (statusCode == HTTP_STATUS_OK)
	{
		Q_ASSERT_X(pReply->isFinished(), "HttpServerStatusParser::parseReply", "Not all data available for reply");
		mVersionInfo = VersionInfo::fromText(QString::fromUtf8(pReply->readAll()));
		qCDebug(network) << "Found version info" << mVersionInfo;
		return !mVersionInfo.isNull();
	}

	qCDebug(network) << "Cannot get status information! Got bad http status code.";
	return false;
}
