/*!
 * \brief Utility class providing information about network connectivity status.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include <QObject>

class test_ConnectivityManager;

namespace governikus
{

class ConnectivityManager
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool networkInterfaceActive READ isNetworkInterfaceActive NOTIFY fireNetworkInterfaceActiveChanged)

	private:
		friend class ::test_ConnectivityManager;
		int mTimerId;
		bool mActive;
		void setActive(bool pActive, const QString& pInterfaceName = QString());
		void updateConnectivity();

	protected:
		void timerEvent(QTimerEvent* pEvent) override;

	public:
		ConnectivityManager(QObject* pParent = nullptr);
		virtual ~ConnectivityManager() override;

		bool isNetworkInterfaceActive() const;
		void startWatching();
		void stopWatching();

	Q_SIGNALS:
		void fireNetworkInterfaceActiveChanged(bool pActive);
};


} // namespace governikus
