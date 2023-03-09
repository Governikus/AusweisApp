/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Utility class to set a timeout on a QNetworkReply
 */

#pragma once

#include "NetworkManager.h"

#include <QNetworkReply>
#include <QTimer>

namespace governikus
{

class NetworkReplyTimeout
	: public QObject
{
	Q_OBJECT

	private:
		QTimer mTimer;

		NetworkReplyTimeout(QNetworkReply* pReply, const int pTimeout);

	private Q_SLOTS:
		void onTimeout();
		void onShutdown();

	public:
		/*!
		 * Set the timeout in milli-seconds on the specified QNetworkReply.
		 */
		static void setTimeout(const NetworkManager* pManager, QNetworkReply* pReply, const int pTimeoutMilliSeconds);
};

} // namespace governikus
