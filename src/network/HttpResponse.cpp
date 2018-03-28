/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "HttpResponse.h"

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
}

Q_DECLARE_LOGGING_CATEGORY(network)

HttpResponse::HttpResponse(HttpStatusCode pStatus, const QByteArray& pBody, const QByteArray& pContentType)
	: mStatus(pStatus)
	, mHeader()
	, mBody()
{
	setBody(pBody, pContentType);

	// According to TR-03124-1, chapter 2.2.2.1, the Server-header shall have the following form:
	auto version = QCoreApplication::applicationVersion().toUtf8();
	if (!version.isEmpty())
	{
		version.prepend('/');
	}
	setHeader(HEADER_SERVER(), QCoreApplication::applicationName().toUtf8() % version % QByteArrayLiteral(" (TR-03124-1/1.3)"));
	setHeader(HEADER_DATE(), QLocale::c().toString(QDateTime::currentDateTimeUtc(), QStringLiteral("ddd, dd MMM yyyy hh:mm:ss")).toUtf8() + QByteArrayLiteral(" GMT"));
	setHeader(HEADER_CONTENT_LENGTH(), QByteArray::number(mBody.size()));
}


HttpResponse::~HttpResponse()
{
}


bool HttpResponse::isValid() const
{
	return mStatus != HttpStatusCode::UNDEFINED;
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


HttpStatusCode HttpResponse::getStatus() const
{
	return mStatus;
}


void HttpResponse::setStatus(HttpStatusCode pStatus)
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


QByteArray HttpResponse::getMessage() const
{
	Q_ASSERT(mStatus != HttpStatusCode::UNDEFINED);

	static const QByteArray CR_LF = QByteArrayLiteral("\r\n");
	QByteArrayList list;

	const auto& statusCode = QByteArray::number(static_cast<int>(mStatus));
	QByteArray statusMsg(getEnumName(mStatus).data());
	list += QByteArrayLiteral("HTTP/1.0 ") % statusCode % ' ' % statusMsg.replace('_', ' ');

	const auto& end = mHeader.constEnd();
	for (auto iter = mHeader.constBegin(); iter != end; ++iter)
	{
		list += iter.key() % QByteArrayLiteral(": ") % iter.value();
	}

	return list.join(CR_LF) % CR_LF % CR_LF % mBody;
}
