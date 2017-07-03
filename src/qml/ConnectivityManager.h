/*!
 * \brief Utility class providing information about network connectivity status.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once


#include <QObject>


namespace governikus
{

class ConnectivityManager
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool networkInterfaceActive READ isNetworkInterfaceActive NOTIFY fireNetworkInterfaceActiveChanged)

	private:
		int mTimerId;
		bool mActive;
		void setActive(bool pActive, const QString& pInterfaceName = QString());
		void updateConnectivity();

	protected:
		void timerEvent(QTimerEvent* pEvent) override;

	public:
		ConnectivityManager(QObject* pParent = nullptr);
		virtual ~ConnectivityManager();

		bool isNetworkInterfaceActive() const;
		void startWatching();
		void stopWatching();

	Q_SIGNALS:
		void fireNetworkInterfaceActiveChanged(bool pActive);
};


} /* namespace governikus */
