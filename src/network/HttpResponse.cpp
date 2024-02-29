/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "HttpResponse.h"

#include "NetworkManager.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QLoggingCategory>
#include <QStringBuilder>

using namespace governikus;

namespace
{
#define HEADER_NAME(_name, _key)\
	QByteArray _name(){\
		return QByteArrayLiteral(_key);\
	}

HEADER_NAME(HEADER_CONTENT_LENGTH, "Content-Length")
HEADER_NAME(HEADER_CONTENT_TYPE, "Content-Type")
HEADER_NAME(HEADER_SERVER, "Server")
HEADER_NAME(HEADER_DATE, "Date")
} // namespace

Q_DECLARE_LOGGING_CATEGORY(network)

HttpResponse::HttpResponse(http_status pStatus, const QByteArray& pBody, const QByteArray& pContentType)
	: mStatus(pStatus)
	, mHeader()
	, mBody()
{
	setBody(pBody, pContentType);
	setHeader(HEADER_SERVER(), NetworkManager::getUserAgentServerHeader().toUtf8());
	setHeader(HEADER_DATE(), QLocale::c().toString(QDateTime::currentDateTimeUtc(), QStringLiteral("ddd, dd MMM yyyy hh:mm:ss")).toUtf8() + QByteArrayLiteral(" GMT"));
	setHeader(HEADER_CONTENT_LENGTH(), QByteArray::number(mBody.size()));
}


QByteArray HttpResponse::getHeader(const QByteArray& pKey) const
{
	return mHeader.value(pKey);
}


const QMap<QByteArray, QByteArray>& HttpResponse::getHeaders() const
{
	return mHeader;
}


void HttpResponse::setHeader(const QByteArray& pKey, const QByteArray& pValue)
{
	mHeader.insert(pKey, pValue);
}


http_status HttpResponse::getStatus() const
{
	return mStatus;
}


void HttpResponse::setStatus(http_status pStatus)
{
	mStatus = pStatus;
}


const QByteArray& HttpResponse::getBody() const
{
	return mBody;
}


void HttpResponse::setBody(const QByteArray& pBody, const QByteArray& pContentType)
{
	mBody = pBody;
	setHeader(HEADER_CONTENT_LENGTH(), QByteArray::number(mBody.size()));
	if (mBody.isEmpty() || pContentType.isEmpty())
	{
		Q_ASSERT(pContentType.isEmpty());
		mHeader.remove(HEADER_CONTENT_TYPE());
	}
	else if (!pContentType.isEmpty())
	{
		setHeader(HEADER_CONTENT_TYPE(), pContentType);
	}
}


QByteArray HttpResponse::getStatusMessage() const
{
	return NetworkManager::getStatusMessage(mStatus);
}


QByteArray HttpResponse::getMessage() const
{
	static const auto CR_LF = QByteArrayLiteral("\r\n");
	QByteArrayList list;
	list += QByteArrayLiteral("HTTP/1.0 ") % QByteArray::number(mStatus) % ' ' % getStatusMessage();

	const auto& end = mHeader.constEnd();
	for (auto iter = mHeader.constBegin(); iter != end; ++iter)
	{
		list += iter.key() % QByteArrayLiteral(": ") % iter.value();
	}

	return list.join(CR_LF) % CR_LF % CR_LF % mBody;
}
