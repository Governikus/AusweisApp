/*!
 * \brief Provides information about the WiFi status
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include <QHostAddress>
#include <QObject>

#if defined(Q_OS_IOS)
#include <Network/Network.h>
#endif

class test_WifiInfo;

namespace governikus
{

class WifiInfo
	: public QObject
{
	Q_OBJECT

	private:
		friend class ::test_WifiInfo;
		bool mWifiEnabled;
#if defined(Q_OS_ANDROID)
		int mWifiCheckTimerId;
#elif defined(Q_OS_IOS)
		nw_path_monitor_t mMonitor;
#endif

		static bool isPrivateIp(const QHostAddress& pAddress);
		static bool hasPrivateIpAddress();

		bool getCurrentWifiEnabled();

#if defined(Q_OS_ANDROID)

	protected:
		void timerEvent(QTimerEvent* pEvent) override;
#endif

	public:
		WifiInfo();
#if defined(Q_OS_IOS)
		virtual ~WifiInfo() override;
#else
		virtual ~WifiInfo() override = default;
#endif

		bool isWifiEnabled();

	Q_SIGNALS:
		void fireWifiEnabledChanged(bool pEnabled);
};


} // namespace governikus
