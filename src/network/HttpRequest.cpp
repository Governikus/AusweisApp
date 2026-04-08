/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

#include "HttpRequest.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)

#define CAST_OBJ(parser) HttpRequest* obj = static_cast<HttpRequest*>(parser->data);

HttpRequest::HttpRequest(QTcpSocket* pSocket, QObject* pParent)
	: QObject(pParent)
	, mUrl()
	, mHeader()
	, mBody()
	, mSocket(pSocket)
	, mParser()
	, mParserSettings()
	, mFinished(false)
	, mCurrentHeaderField()
	, mCurrentHeaderValue()
{
	Q_ASSERT(mSocket);
	mSocket->setParent(this);
	mSocket->startTransaction();

	llhttp_settings_init(&mParserSettings);
	mParserSettings.on_message_complete = &HttpRequest::onMessageComplete;
	mParserSettings.on_headers_complete = &HttpRequest::onHeadersComplete;
	mParserSettings.on_header_field = &HttpRequest::onHeaderField;
	mParserSettings.on_header_value = &HttpRequest::onHeaderValue;
	mParserSettings.on_body = &HttpRequest::onBody;
	mParserSettings.on_url = &HttpRequest::onUrl;

	llhttp_init(&mParser, HTTP_REQUEST, &mParserSettings);
	mParser.data = this;

	connect(mSocket, &QAbstractSocket::readyRead, this, &HttpRequest::onReadyRead);
	connect(mSocket, &QAbstractSocket::stateChanged, this, &HttpRequest::fireSocketStateChanged);
}


QTcpSocket* HttpRequest::take()
{
	mSocket->disconnect(this);
	auto socket = mSocket;
	socket->setParent(nullptr);
	mSocket.clear();
	return socket;
}


HttpRequest::~HttpRequest()
{
	if (mSocket && mSocket->bytesToWrite() && mSocket->state() == QAbstractSocket::ConnectedState && !mSocket->flush())
	{
		qCDebug(network) << "Flush socket failed. Waiting for bytes:" << mSocket->bytesToWrite();
		if (!mSocket->waitForBytesWritten())
		{
			qCWarning(network) << "Cannot wait for socket anymore... abort!";
		}
	}
}


bool HttpRequest::isConnected() const
{
	return mSocket && mSocket->state() == QAbstractSocket::ConnectedState;
}


QByteArray HttpRequest::getMethod() const
{
	return QByteArray(llhttp_method_name(getHttpMethod()));
}


llhttp_method HttpRequest::getHttpMethod() const
{
	return static_cast<llhttp_method>(mParser.method);
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


quint16 HttpRequest::getPeerPort() const
{
	return mSocket->peerPort();
}


quint16 HttpRequest::getLocalPort() const
{
	return mSocket->localPort();
}


void HttpRequest::triggerSocketBuffer()
{
	onReadyRead();
}


bool HttpRequest::send(llhttp_status pStatus)
{
	return send(HttpResponse(pStatus));
}


bool HttpRequest::send(const HttpResponse& pResponse)
{
	return send(pResponse.getMessage());
}


bool HttpRequest::send(const QByteArray& pResponse)
{
	if (!mSocket)
	{
		return false;
	}

	if (mSocket->write(pResponse) != pResponse.size())
	{
		qCCritical(network) << "Cannot write response:" << mSocket->error() << '|' << mSocket->errorString();
		return false;
	}
	mSocket->flush();
	return true;
}


void HttpRequest::onReadyRead()
{
	if (!mSocket)
	{
		return;
	}

	if (mFinished)
	{
		while (mSocket->bytesAvailable())
		{
			Q_EMIT fireSocketBuffer(mSocket->readAll());
		}
		return;
	}

	while (mSocket->bytesAvailable())
	{
		const auto& buffer = mSocket->readAll();
		const auto errorCode = llhttp_execute(&mParser, buffer.constData(), static_cast<size_t>(buffer.size()));
		if (errorCode != HPE_OK)
		{
			qCWarning(network) << "Http request not well-formed:" << llhttp_errno_name(errorCode) << '|' << llhttp_get_error_reason(&mParser);
		}
	}

	if (mFinished)
	{
		mSocket->rollbackTransaction();
		if (!isUpgrade())
		{
			disconnect(mSocket, &QAbstractSocket::readyRead, this, &HttpRequest::onReadyRead);
		}
		Q_EMIT fireMessageComplete(this);
	}
}


int HttpRequest::onMessageComplete(llhttp_t* pParser)
{
	CAST_OBJ(pParser)
	obj->mFinished = true;
	qCDebug(network) << "Message completed";
	return 0;
}


int HttpRequest::onHeadersComplete(llhttp_t* pParser)
{
	CAST_OBJ(pParser)
	obj->insertHeader();
	qCDebug(network) << obj->getMethod() << '|' << obj->getUrl();
	qCDebug(network) << "Header completed";
	return 0;
}


int HttpRequest::onHeaderField(llhttp_t* pParser, const char* const pPos, size_t pLength)
{
	CAST_OBJ(pParser)
	obj->insertHeader();
	add(obj->mCurrentHeaderField, pPos, pLength);
	return 0;
}


int HttpRequest::onHeaderValue(llhttp_t* pParser, const char* const pPos, size_t pLength)
{
	CAST_OBJ(pParser)
	add(obj->mCurrentHeaderValue, pPos, pLength);
	return 0;
}


int HttpRequest::onBody(llhttp_t* pParser, const char* const pPos, size_t pLength)
{
	CAST_OBJ(pParser)
	add(obj->mBody, pPos, pLength);
	return 0;
}


int HttpRequest::onUrl(llhttp_t* pParser, const char* const pPos, size_t pLength)
{
	CAST_OBJ(pParser)
	add(obj->mUrl, pPos, pLength);
	return 0;
}


void HttpRequest::insertHeader()
{
	if (!mCurrentHeaderField.isEmpty() && !mCurrentHeaderValue.isNull())
	{
		qCDebug(network).nospace() << "Header | " << mCurrentHeaderField << ": " << mCurrentHeaderValue;
		mHeader.insert(mCurrentHeaderField.toLower(), mCurrentHeaderValue);
		mCurrentHeaderField.clear();
		mCurrentHeaderValue.clear();
	}
}
