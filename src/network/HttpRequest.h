/*!
 * \brief Class to parse http request.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpResponse.h"

#include <http_parser.h>
#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QScopedPointer>
#include <QTcpSocket>
#include <QUrl>

class test_WebserviceActivationHandler;
class test_WebserviceActivationContext;

namespace governikus
{

class HttpRequest
	: public QObject
{
	Q_OBJECT

	private:
		friend class ::test_WebserviceActivationHandler;
		friend class ::test_WebserviceActivationContext;
		friend class HttpServer;

		static int onMessageBegin(http_parser* pParser);
		static int onMessageComplete(http_parser* pParser);
		static int onHeadersComplete(http_parser* pParser);
		static int onHeaderField(http_parser* pParser, const char* const pPos, size_t pLength);
		static int onHeaderValue(http_parser* pParser, const char* const pPos, size_t pLength);
		static int onBody(http_parser* pParser, const char* const pPos, size_t pLength);
		static int onUrl(http_parser* pParser, const char* const pPos, size_t pLength);

		static inline void add(QByteArray& pDest, const char* const pPos, size_t pLength)
		{
			pDest += QByteArray(pPos, static_cast<int>(pLength));
		}


		QByteArray mUrl;
		QMap<QByteArray, QByteArray> mHeader;
		QByteArray mBody;
		QScopedPointer<QTcpSocket, QScopedPointerDeleteLater> mSocket;
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
