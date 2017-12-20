/*!
 * \brief Provides information about the Wifi status
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include <QObject>

namespace governikus
{

class WifiInfo
	: public QObject
{
	Q_OBJECT

	private:
		bool mWifiEnabled;
		int mWifiEnabledTimerId;

		bool getCurrentWifiEnabled();

	protected:
		void timerEvent(QTimerEvent* pEvent) override;

	public:
		WifiInfo();
		virtual ~WifiInfo() override;

		bool isWifiEnabled();

	Q_SIGNALS:
		void fireWifiEnabledChanged(bool pEnabled);

};


}
