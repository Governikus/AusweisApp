/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "NetworkReplyTimeout.h"

#include <QCoreApplication>

using namespace governikus;

NetworkReplyTimeout::NetworkReplyTimeout(QNetworkReply* pReply, const int pTimeout)
	: QObject(pReply)
{
	Q_ASSERT(pReply);
	if (pReply == nullptr)
	{
		return;
	}

	connect(&mTimer, &QTimer::timeout, this, &NetworkReplyTimeout::onTimeout);
	mTimer.setSingleShot(true);
	mTimer.setInterval(pTimeout);
	mTimer.start();
}


void NetworkReplyTimeout::onTimeout()
{
	auto* reply = static_cast<QNetworkReply*>(parent());
	if (reply != nullptr && reply->isRunning())
	{
		reply->abort();
	}
}


void NetworkReplyTimeout::onShutdown()
{
	mTimer.stop();
	onTimeout();
}


void NetworkReplyTimeout::setTimeout(const NetworkManager* pManager, QNetworkReply* pReply, const int pTimeout)
{
	// since the QNetworkReply is set as parent, we don't need to care about destruction
	const auto* timeout = new NetworkReplyTimeout(pReply, pTimeout);
	connect(pManager, &NetworkManager::fireShutdown, timeout, &NetworkReplyTimeout::onShutdown);
}
