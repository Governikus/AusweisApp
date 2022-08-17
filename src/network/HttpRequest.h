/*!
 * \brief Class to parse http request.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpResponse.h"

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QScopedPointer>
#include <QTcpSocket>
#include <QUrl>

#include <http_parser.h>

#include <memory>

class test_WebserviceActivationHandler;
class test_WebserviceActivationContext;

namespace governikus
{

class HttpRequest
	: public QObject
{
	Q_OBJECT
	friend class ::test_WebserviceActivationHandler;
	friend class ::test_WebserviceActivationContext;
	friend class HttpServer;

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
#if (QT_VERSION < QT_VERSION_CHECK(6, 1, 0))
		QScopedPointer<QTcpSocket, QScopedPointerDeleteLater> mSocket;
#else
		std::unique_ptr<QTcpSocket, QScopedPointerDeleteLater> mSocket;
#endif
		http_parser mParser;
		http_parser_settings mParserSettings;

		bool mSocketDisconnected;
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

		bool send(const HttpResponse& pResponse);

		QTcpSocket* take();

	private Q_SLOTS:
		void onReadyRead();
		void onSocketDisconnected();

	Q_SIGNALS:
		void fireMessageComplete(HttpRequest* pSelf);
};

} // namespace governikus
