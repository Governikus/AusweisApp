/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Utility class providing information about network connectivity status.
 */

#pragma once

#include <QHostAddress>
#include <QList>
#include <QObject>
#include <QtQml/qqmlregistration.h>

class test_ConnectivityManager;


namespace governikus
{

class ConnectivityManager
	: public QObject
{
	Q_OBJECT
	QML_ELEMENT

	friend class ::test_ConnectivityManager;

	Q_PROPERTY(bool watching READ isWatching WRITE setWatching NOTIFY fireWatchingChanged)
	Q_PROPERTY(bool networkInterfaceActive READ isNetworkInterfaceActive NOTIFY fireNetworkInterfaceActiveChanged)

	private:
		int mTimerId;
		bool mActive;
		QList<QHostAddress> mAllAddresses;

		void setActive(bool pActive);
		void timerEvent(QTimerEvent* pEvent) override;

	public:
		ConnectivityManager();
		~ConnectivityManager() override;

		[[nodiscard]] bool isWatching() const;
		void setWatching(bool pWatching);
		Q_INVOKABLE bool checkConnectivity();
		[[nodiscard]] bool isNetworkInterfaceActive() const;

	Q_SIGNALS:
		void fireWatchingChanged();
		void fireNetworkInterfaceActiveChanged(bool pActive);
};


} // namespace governikus
