/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Mock a QNetworkReply for tests.
 */

#pragma once

#include "MockSocket.h"

#include <QNetworkReply>
#include <http_parser.h>


namespace governikus
{

class MockNetworkReply
	: public QNetworkReply
{
	Q_OBJECT

	private:
		MockSocket mSocket;

	public:
		MockNetworkReply(const QByteArray& pData = QByteArray(), http_status pStatusCode = HTTP_STATUS_OK, QObject* pParent = nullptr);
		~MockNetworkReply() override;

		void abort() override
		{
			qDebug() << "Operation aborted";
			fireFinished();
		}


		void setAttribute(QNetworkRequest::Attribute pCode, const QVariant& pValue)
		{
			QNetworkReply::setAttribute(pCode, pValue);
		}


		void setError(QNetworkReply::NetworkError pErrorCode, const QString& pErrorString)
		{
			QNetworkReply::setError(pErrorCode, pErrorString);
		}


		void setRawHeader(const QByteArray& pHeaderName, const QByteArray& pValue)
		{
			QNetworkReply::setRawHeader(pHeaderName, pValue);
		}


		void setRequest(const QNetworkRequest& pRequest)
		{
			QNetworkReply::setRequest(pRequest);
		}


		qint64 readData(char* pDst, qint64 pMaxSize) override;

		void fireFinished();


		void setFileModificationTimestamp(const QVariant& pTimestamp)
		{
			setHeader(QNetworkRequest::KnownHeaders::LastModifiedHeader, pTimestamp);
		}


};

} // namespace governikus
