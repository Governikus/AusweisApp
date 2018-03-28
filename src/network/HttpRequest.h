/*!
 * \brief Class to parse http request.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "http_parser.h"
#include "HttpResponse.h"

#include <QAbstractSocket>
#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QUrl>

class test_WebserviceActivationHandler;

namespace governikus
{

class HttpRequest
	: public QObject
{
	Q_OBJECT

	private:
		friend class ::test_WebserviceActivationHandler;

		static int onMessageBegin(http_parser* pParser);
		static int onMessageComplete(http_parser* pParser);
		static int onHeadersComplete(http_parser* pParser);
		static int onHeaderField(http_parser* pParser, const char* pPos, size_t pLength);
		static int onHeaderValue(http_parser* pParser, const char* pPos, size_t pLength);
		static int onBody(http_parser* pParser, const char* pPos, size_t pLength);
		static int onUrl(http_parser* pParser, const char* pPos, size_t pLength);

		static inline void add(QByteArray& pDest, const char* pPos, size_t pLength)
		{
			pDest += QByteArray(pPos, static_cast<int>(pLength));
		}


		QByteArray mUrl;
		QMap<QByteArray, QByteArray> mHeader;
		QByteArray mBody;
		QSharedPointer<QAbstractSocket> mSocket;
		http_parser mParser;
		http_parser_settings mParserSettings;

		bool mFinished;
		QByteArray mCurrentHeaderField;
		QByteArray mCurrentHeaderValue;

		void insertHeader();

	public:
		HttpRequest(QAbstractSocket* pSocket, QObject* pParent = nullptr);
		virtual ~HttpRequest();

		bool isConnected() const;

		QByteArray getMethod() const;
		bool isUpgrade() const;
		QByteArray getHeader(const QByteArray& pKey) const;
		const QMap<QByteArray, QByteArray>& getHeader() const;
		QUrl getUrl() const;
		const QByteArray& getBody() const;

		bool send(const HttpResponse& pResponse);

	private Q_SLOTS:
		void onReadyRead();

	Q_SIGNALS:
		void fireMessageComplete(HttpRequest* pSelf, QSharedPointer<QAbstractSocket> pSocket);
};

} /* namespace governikus */
