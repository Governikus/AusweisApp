/*!
 * \brief Provides information about the Wifi status
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
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
		int mWifiCheckTimerId;
		int mWifiEnableWaitCounter;

		static bool isPrivateIp(const QHostAddress& pAddress);
		static bool hasPrivateIpAddress();

		bool getCurrentWifiEnabled();
		bool shouldWifiEnabledBeCalled();

	protected:
		void timerEvent(QTimerEvent* pEvent) override;

	public:
		WifiInfo();
		virtual ~WifiInfo() override = default;

		bool isWifiEnabled();
		void enableWifi();

	Q_SIGNALS:
		void fireWifiEnabledChanged(bool pEnabled);

};


} // namespace governikus
