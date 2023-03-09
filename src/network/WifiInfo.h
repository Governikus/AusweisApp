/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Provides information about the WiFi status
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
	friend class ::test_WifiInfo;

	private:
		bool mWifiEnabled;
#if defined(Q_OS_ANDROID)
		int mWifiCheckTimerId;
#endif

		static bool isPrivateIp(const QHostAddress& pAddress);
		static bool hasPrivateIpAddress();

		[[nodiscard]] bool getCurrentWifiEnabled();

#if defined(Q_OS_ANDROID)

	protected:
		void timerEvent(QTimerEvent* pEvent) override;
#endif

	public:
		WifiInfo();
		~WifiInfo() override = default;

		[[nodiscard]] bool isWifiEnabled();

	Q_SIGNALS:
		void fireWifiEnabledChanged(bool pEnabled);
};


} // namespace governikus
