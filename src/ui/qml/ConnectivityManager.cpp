/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ConnectivityManager.h"

#include <QLoggingCategory>
#include <QNetworkInterface>
#include <QTimerEvent>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


ConnectivityManager::ConnectivityManager(QObject* pParent)
	: QObject(pParent)
	, mTimerId(0)
	, mActive(false)
{
}


ConnectivityManager::~ConnectivityManager()
{
	if (mTimerId)
	{
		killTimer(mTimerId);
	}
}


void ConnectivityManager::setActive(bool pActive, const QString& pInterfaceName)
{
	if (mActive != pActive)
	{
		if (pActive)
		{
			qCDebug(network) << "Found active network interface" << pInterfaceName;
		}
		else
		{
			qCDebug(network) << "Found no active network interface";
		}
		mActive = pActive;
		Q_EMIT fireNetworkInterfaceActiveChanged(mActive);
	}
}


void ConnectivityManager::updateConnectivity()
{
	const auto& allInterfaces = QNetworkInterface::allInterfaces();
	for (const auto& iface : allInterfaces)
	{
		if (iface.flags().testFlag(QNetworkInterface::IsUp)
				&& iface.flags().testFlag(QNetworkInterface::IsRunning)
				&& !iface.flags().testFlag(QNetworkInterface::IsLoopBack)
				&& !iface.addressEntries().isEmpty())
		{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
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
#endif

			setActive(true, iface.name());
			return;
		}
	}
	setActive(false);
}


void ConnectivityManager::timerEvent(QTimerEvent* pTimerEvent)
{
	if (pTimerEvent->timerId() == mTimerId)
	{
		updateConnectivity();
	}
}


bool ConnectivityManager::isNetworkInterfaceActive() const
{
	return mActive;
}


void ConnectivityManager::startWatching()
{
	if (mTimerId)
	{
		qCWarning(network) << "Already started, skip";
		return;
	}
	mTimerId = startTimer(1000);
	updateConnectivity();
}


void ConnectivityManager::stopWatching()
{
	if (!mTimerId)
	{
		qCWarning(network) << "Already stopped, skip";
		return;
	}
	killTimer(mTimerId);
	mTimerId = 0;
}
