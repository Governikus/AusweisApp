/*!
 * \brief Utility class providing information about network connectivity status.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "Env.h"

#include <QObject>

class test_ConnectivityManager;

namespace governikus
{

class ConnectivityManager
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_ConnectivityManager;

	Q_PROPERTY(bool networkInterfaceActive READ isNetworkInterfaceActive NOTIFY fireNetworkInterfaceActiveChanged)

	private:
		int mTimerId;
		bool mActive;

		ConnectivityManager();
		~ConnectivityManager() override;

		void setActive(bool pActive, const QString& pInterfaceName = QString());
		void updateConnectivity();
		void timerEvent(QTimerEvent* pEvent) override;

	public:
		bool isNetworkInterfaceActive() const;
		void startWatching();
		void stopWatching();

	Q_SIGNALS:
		void fireNetworkInterfaceActiveChanged(bool pActive);
};


} // namespace governikus
