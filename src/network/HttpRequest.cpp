/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "HttpRequest.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)

#define CAST_OBJ(parser) HttpRequest * obj = static_cast<HttpRequest*>(parser->data);

HttpRequest::HttpRequest(QAbstractSocket* pSocket, QObject* pParent)
	: QObject(pParent)
	, mUrl()
	, mHeader()
	, mBody()
	, mSocket(pSocket, [](QAbstractSocket* s){
				s->flush();
				s->deleteLater();
			})
	, mParser()
	, mParserSettings()
	, mFinished(false)
	, mCurrentHeaderField()
	, mCurrentHeaderValue()
{
	Q_ASSERT(mSocket);

	http_parser_init(&mParser, HTTP_REQUEST);
	mParser.data = this;

	mParserSettings.on_message_begin = &HttpRequest::onMessageBegin;
	mParserSettings.on_message_complete = &HttpRequest::onMessageComplete;
	mParserSettings.on_headers_complete = &HttpRequest::onHeadersComplete;
	mParserSettings.on_header_field = &HttpRequest::onHeaderField;
	mParserSettings.on_header_value = &HttpRequest::onHeaderValue;
	mParserSettings.on_body = &HttpRequest::onBody;
	mParserSettings.on_url = &HttpRequest::onUrl;

	connect(mSocket.data(), &QAbstractSocket::readyRead, this, &HttpRequest::onReadyRead);
	connect(mSocket.data(), &QAbstractSocket::disconnected, this, &HttpRequest::deleteLater);
	onReadyRead();
}


HttpRequest::~HttpRequest()
{
}


bool HttpRequest::isConnected() const
{
	return mSocket->state() == QAbstractSocket::ConnectedState;
}


QByteArray HttpRequest::getMethod() const
{
	return QByteArray(http_method_str(static_cast<http_method>(mParser.method)));
}


bool HttpRequest::isUpgrade() const
{
	return mParser.upgrade;
}


QByteArray HttpRequest::getHeader(const QByteArray& pKey) const
{
	return mHeader.value(pKey);
}


const QMap<QByteArray, QByteArray>& HttpRequest::getHeader() const
{
	return mHeader;
}


QUrl HttpRequest::getUrl() const
{
	return QUrl::fromEncoded(mUrl);
}


const QByteArray& HttpRequest::getBody() const
{
	return mBody;
}


bool HttpRequest::send(const HttpResponse& pResponse)
{
	const auto& msg = pResponse.getMessage();
	if (mSocket->write(msg) != msg.size())
	{
		qCCritical(network) << "Cannot write response:" << mSocket->error() << '|' << mSocket->errorString();
		return false;
	}
	return true;
}


void HttpRequest::onReadyRead()
{
	while (mSocket->bytesAvailable())
	{
		const auto& buffer = mSocket->readAll();
		http_parser_execute(&mParser, &mParserSettings, buffer.constData(), static_cast<size_t>(buffer.size()));

		// See macro HTTP_PARSER_ERRNO if http_errno fails.
		// We do not use this to avoid -Wold-style-cast warning
		const auto errorCode = static_cast<http_errno>(mParser.http_errno);
		if (errorCode != HPE_OK)
		{
			qCWarning(network) << "Http request not well-formed:" << http_errno_name(errorCode) << "|" << http_errno_description(errorCode);
		}
	}

	if (mFinished)
	{
		disconnect(mSocket.data(), &QAbstractSocket::readyRead, this, &HttpRequest::onReadyRead);
		disconnect(mSocket.data(), &QAbstractSocket::disconnected, this, &HttpRequest::deleteLater);
		Q_EMIT fireMessageComplete(this, mSocket);
	}
}


int HttpRequest::onMessageBegin(http_parser* pParser)
{
	Q_UNUSED(pParser)
	return 0;
}


int HttpRequest::onMessageComplete(http_parser* pParser)
{
	CAST_OBJ(pParser)
	obj->mFinished = true;
	qCDebug(network) << "Message completed";
	return 0;
}


int HttpRequest::onHeadersComplete(http_parser* pParser)
{
	CAST_OBJ(pParser)
	obj->insertHeader();
	qCDebug(network) << obj->getMethod() << " |" << obj->getUrl();
	qCDebug(network) << "Header completed";
	return 0;
}


int HttpRequest::onHeaderField(http_parser* pParser, const char* pPos, size_t pLength)
{
	CAST_OBJ(pParser)
	obj->insertHeader();
	add(obj->mCurrentHeaderField, pPos, pLength);
	return 0;
}


int HttpRequest::onHeaderValue(http_parser* pParser, const char* pPos, size_t pLength)
{
	CAST_OBJ(pParser)
	add(obj->mCurrentHeaderValue, pPos, pLength);
	return 0;
}


int HttpRequest::onBody(http_parser* pParser, const char* pPos, size_t pLength)
{
	CAST_OBJ(pParser)
	add(obj->mBody, pPos, pLength);
	return 0;
}


int HttpRequest::onUrl(http_parser* pParser, const char* pPos, size_t pLength)
{
	CAST_OBJ(pParser)
	add(obj->mUrl, pPos, pLength);
	return 0;
}


void HttpRequest::insertHeader()
{
	if (!mCurrentHeaderField.isEmpty() && !mCurrentHeaderValue.isEmpty())
	{
		qCDebug(network).nospace() << "Header | " << mCurrentHeaderField << ": " << mCurrentHeaderValue;
		mHeader.insert(mCurrentHeaderField.toLower(), mCurrentHeaderValue);
		mCurrentHeaderField.clear();
		mCurrentHeaderValue.clear();
	}
}
