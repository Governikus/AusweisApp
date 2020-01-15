/*!
 * \brief Provides information about the Wifi status
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
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

		static bool isPrivateIp(const QHostAddress& pAddress);
		static bool hasPrivateIpAddress();

		bool getCurrentWifiEnabled();

	protected:
		void timerEvent(QTimerEvent* pEvent) override;

	public:
		WifiInfo();
		virtual ~WifiInfo() override = default;

		bool isWifiEnabled();

	Q_SIGNALS:
		void fireWifiEnabledChanged(bool pEnabled);
};


} // namespace governikus
