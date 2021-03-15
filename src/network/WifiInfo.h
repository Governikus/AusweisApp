/*!
 * \brief Provides information about the WiFi status
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include <QHostAddress>
#include <QObject>

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
		~WifiInfo() override = default;

		bool isWifiEnabled();

	Q_SIGNALS:
		void fireWifiEnabledChanged(bool pEnabled);
};


} // namespace governikus
