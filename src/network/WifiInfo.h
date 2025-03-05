/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
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

		[[nodiscard]] bool getCurrentWifiEnabled() const;

#if defined(Q_OS_ANDROID)

	protected:
		void timerEvent(QTimerEvent* pEvent) override;
#endif

	public:
		WifiInfo();
		~WifiInfo() override = default;

		[[nodiscard]] bool isWifiEnabled() const;

	Q_SIGNALS:
		void fireWifiEnabledChanged(bool pEnabled);
};


} // namespace governikus
