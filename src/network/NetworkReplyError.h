/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
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
		qint64 readData(char* data, qint64 maxlen) override;

	public Q_SLOTS:
		void abort() override;
		void onErrorSignals();

	public:
		explicit NetworkReplyError(const QNetworkRequest& pRequest, QObject* pParent = nullptr);
};

} // namespace governikus
