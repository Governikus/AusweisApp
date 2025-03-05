/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ConnectivityManager.h"

#include <QLoggingCategory>
#include <QNetworkInterface>
#include <QTimerEvent>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


void ConnectivityManager::setActive(bool pActive)
{
	if (mActive != pActive)
	{
		if (pActive)
		{
			qCDebug(network) << "A network interface is now available";
		}
		else
		{
			qCDebug(network) << "An active network interface is no longer available";
		}
		mActive = pActive;
		Q_EMIT fireNetworkInterfaceActiveChanged(mActive);
	}
}


void ConnectivityManager::timerEvent(QTimerEvent* pTimerEvent)
{
	if (pTimerEvent->timerId() == mTimerId)
	{
		setActive(checkConnectivity());
	}
}


ConnectivityManager::ConnectivityManager()
	: QObject()
	, mTimerId(0)
	, mActive(false)
{
}


ConnectivityManager::~ConnectivityManager()
{
	if (isWatching())
	{
		killTimer(mTimerId);
	}
}


bool ConnectivityManager::isWatching() const
{
	return mTimerId != 0;
}


void ConnectivityManager::setWatching(bool pWatching)
{
	if (pWatching == isWatching())
	{
		return;
	}

	if (pWatching)
	{
		mTimerId = startTimer(1000);
		Q_EMIT fireWatchingChanged();
		setActive(checkConnectivity());
		return;
	}

	killTimer(mTimerId);
	mTimerId = 0;
	Q_EMIT fireWatchingChanged();
}


bool ConnectivityManager::checkConnectivity()
{
	const auto& allInterfaces = QNetworkInterface::allInterfaces();
	for (const auto& iface : allInterfaces)
	{
		if (!iface.flags().testFlag(QNetworkInterface::IsUp)
				|| !iface.flags().testFlag(QNetworkInterface::IsRunning)
				|| iface.flags().testFlag(QNetworkInterface::IsLoopBack)
				|| iface.addressEntries().isEmpty())
		{
			continue;
		}

		bool hasNonLocalAddress = false;
		const auto& entries = iface.addressEntries();
		for (const auto& entry : entries)
		{
			const QHostAddress& addr = entry.ip();
			if (addr.protocol() == QAbstractSocket::IPv4Protocol
					|| !addr.isLinkLocal())
			{
				hasNonLocalAddress = true;
				break;
			}
		}

		if (!hasNonLocalAddress)
		{
			continue;
		}

		if (!mActive)
		{
			qCDebug(network) << "Found active network interface" << iface.name();
		}
		return true;
	}

	// QNetworkInterface has no operator== so we use allAddresses()
	// to check if something changed. We don't want to log
	// all interfaces for every check here.
	const auto& addresses = QNetworkInterface::allAddresses();
	if (mAllAddresses != addresses)
	{
		mAllAddresses = addresses;
		qCDebug(network) << "No active network interface found";
		for (const auto& interface : allInterfaces)
		{
			qCDebug(network) << interface;
		}
	}

	return false;
}


bool ConnectivityManager::isNetworkInterfaceActive() const
{
	return mActive;
}
