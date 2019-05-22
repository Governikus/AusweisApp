/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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
		virtual qint64 readData(char* data, qint64 maxlen) override;

	public Q_SLOTS:
		virtual void abort() override;
		void onErrorSignals();

	public:
		explicit NetworkReplyError(const QNetworkRequest& pRequest, QObject* pParent = nullptr);
};

} // namespace governikus
