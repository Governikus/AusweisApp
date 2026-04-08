/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpResponse.h"

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QUrl>

#include <llhttp.h>

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
		[[nodiscard]] static int onMessageComplete(llhttp_t* pParser);
		[[nodiscard]] static int onHeadersComplete(llhttp_t* pParser);
		[[nodiscard]] static int onHeaderField(llhttp_t* pParser, const char* const pPos, size_t pLength);
		[[nodiscard]] static int onHeaderValue(llhttp_t* pParser, const char* const pPos, size_t pLength);
		[[nodiscard]] static int onBody(llhttp_t* pParser, const char* const pPos, size_t pLength);
		[[nodiscard]] static int onUrl(llhttp_t* pParser, const char* const pPos, size_t pLength);

		static inline void add(QByteArray& pDest, const char* const pPos, size_t pLength)
		{
			pDest += QByteArray(pPos, static_cast<int>(pLength));
		}


		QByteArray mUrl;
		QMap<QByteArray, QByteArray> mHeader;
		QByteArray mBody;
		QPointer<QTcpSocket> mSocket;
		llhttp_t mParser;
		llhttp_settings_t mParserSettings;

		bool mFinished;
		QByteArray mCurrentHeaderField;
		QByteArray mCurrentHeaderValue;

		void insertHeader();

	public:
		explicit HttpRequest(QTcpSocket* pSocket, QObject* pParent = nullptr);
		~HttpRequest() override;

		[[nodiscard]] bool isConnected() const;

		[[nodiscard]] QByteArray getMethod() const;
		[[nodiscard]] llhttp_method getHttpMethod() const;
		[[nodiscard]] bool isUpgrade() const;
		[[nodiscard]] QByteArray getHeader(const QByteArray& pKey) const;
		[[nodiscard]] const QMap<QByteArray, QByteArray>& getHeader() const;
		[[nodiscard]] QUrl getUrl() const;
		[[nodiscard]] const QByteArray& getBody() const;
		[[nodiscard]] quint16 getPeerPort() const;
		[[nodiscard]] quint16 getLocalPort() const;
		void triggerSocketBuffer();

		bool send(llhttp_status pStatus);
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
