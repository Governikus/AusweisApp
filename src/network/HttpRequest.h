/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class to parse http request.
 */

#pragma once

#include "HttpResponse.h"

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QUrl>

#include <http_parser.h>

#include <memory>


class test_HttpRequest;


namespace governikus
{

class HttpRequest
	: public QObject
{
	Q_OBJECT
	friend class HttpServer;
	friend class ::test_HttpRequest;

	private:
		[[nodiscard]] static int onMessageBegin(http_parser* pParser);
		[[nodiscard]] static int onMessageComplete(http_parser* pParser);
		[[nodiscard]] static int onHeadersComplete(http_parser* pParser);
		[[nodiscard]] static int onHeaderField(http_parser* pParser, const char* const pPos, size_t pLength);
		[[nodiscard]] static int onHeaderValue(http_parser* pParser, const char* const pPos, size_t pLength);
		[[nodiscard]] static int onBody(http_parser* pParser, const char* const pPos, size_t pLength);
		[[nodiscard]] static int onUrl(http_parser* pParser, const char* const pPos, size_t pLength);

		static inline void add(QByteArray& pDest, const char* const pPos, size_t pLength)
		{
			pDest += QByteArray(pPos, static_cast<int>(pLength));
		}


		QByteArray mUrl;
		QMap<QByteArray, QByteArray> mHeader;
		QByteArray mBody;
		QPointer<QTcpSocket> mSocket;
		http_parser mParser;
		http_parser_settings mParserSettings;

		bool mFinished;
		QByteArray mCurrentHeaderField;
		QByteArray mCurrentHeaderValue;

		void insertHeader();

	public:
		HttpRequest(QTcpSocket* pSocket, QObject* pParent = nullptr);
		~HttpRequest() override;

		[[nodiscard]] bool isConnected() const;

		[[nodiscard]] QByteArray getMethod() const;
		[[nodiscard]] http_method getHttpMethod() const;
		[[nodiscard]] bool isUpgrade() const;
		[[nodiscard]] QByteArray getHeader(const QByteArray& pKey) const;
		[[nodiscard]] const QMap<QByteArray, QByteArray>& getHeader() const;
		[[nodiscard]] QUrl getUrl() const;
		[[nodiscard]] const QByteArray& getBody() const;
		[[nodiscard]] quint16 getPeerPort() const;
		[[nodiscard]] quint16 getLocalPort() const;
		void triggerSocketBuffer();

		bool send(const HttpResponse& pResponse);
		bool send(const QByteArray& pResponse);

		QTcpSocket* take();

	private Q_SLOTS:
		void onReadyRead();

	Q_SIGNALS:
		void fireMessageComplete(HttpRequest* pSelf);
		void fireSocketStateChanged(QAbstractSocket::SocketState pSocketState);
		void fireSocketBuffer(const QByteArray& pBuffer);
};

} // namespace governikus
