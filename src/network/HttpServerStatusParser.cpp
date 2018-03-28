/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "HttpServerStatusParser.h"

#include "HttpStatusCode.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


HttpServerStatusParser::HttpServerStatusParser(int pPort, const QHostAddress& pHost)
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

	QPointer<QNetworkReply> reply = mRequestor.request(mUrl);
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


bool HttpServerStatusParser::parseReply(const QPointer<QNetworkReply>& pReply)
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

	int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == HttpStatusCode::OK)
	{
		Q_ASSERT_X(pReply->isFinished(), "HttpServerStatusParser::parseReply", "Not all data available for reply");
		mVersionInfo = VersionInfo::fromText(QString::fromUtf8(pReply->readAll()));
		qCDebug(network) << "Found version info" << mVersionInfo;
		return !mVersionInfo.isNull();
	}
	else
	{
		qCDebug(network) << "Cannot get status information! Got bad http status code:" << statusCode;
	}

	return false;
}
