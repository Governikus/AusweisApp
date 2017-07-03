/*
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QNetworkReply>

namespace governikus
{

class NetworkReplyError
	: public QNetworkReply
{
	Q_OBJECT

	protected:
		virtual qint64 readData(char* data, qint64 maxlen);

	public Q_SLOTS:
		virtual void abort();
		void onErrorSignals();

	public:
		explicit NetworkReplyError(const QNetworkRequest& pRequest, QObject* pParent = nullptr);
};

} /* namespace governikus */
