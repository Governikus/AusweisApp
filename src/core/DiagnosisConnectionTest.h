/*!
 * \brief Class for retrieving informations about the system proxy, pinging the proxy, trying
 * to establish a connection to a test server with and without the proxy and providing the
 * results to the DiagnosisModel.
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QNetworkProxy>
#include <QObject>
#include <QTcpSocket>

class test_DiagnosisConnectionTest;

namespace governikus
{

class DiagnosisConnectionTest
	: public QObject
{
	Q_OBJECT

	private:
		friend class ::test_DiagnosisConnectionTest;
		bool mIsProxySet;
		QString mProxyHostName;
		QString mProxyPort;
		QString mProxyType;
		QString mProxyCapabilities;

		bool mConnectionTestWithProxySuccessful;
		bool mConnectionTestWithoutProxySuccessful;

		bool mPingTestOnProxySuccessful;

		bool mProxyPingDone;
		bool mConnectionTestWithProxyDone;
		bool mConnectionTestWithoutProxyDone;

		QTcpSocket mPingSocketToProxy;
		QTcpSocket mTcpSocketWithProxy;
		QTcpSocket mTcpSocketWithoutProxy;

		void checkIfAllProcessesDone();
		static QString getProxyTypeAsQString(QNetworkProxy::ProxyType pType);
		static QString getProxyCapabilitiesAsQString(QNetworkProxy::Capabilities pCaps);

	private Q_SLOTS:
		void onProxyPingTestDone();
		void onProxyPingTestError(QAbstractSocket::SocketError pSocketError);
		void onSocketConnectionTestWithProxyDone();
		void onSocketConnectionTestWithProxyError(QAbstractSocket::SocketError pSocketError);
		void onSocketConnectionTestWithoutProxyDone();
		void onSocketConnectionTestWithoutProxyError(QAbstractSocket::SocketError pSocketError);

	public:
		DiagnosisConnectionTest();
		void startConnectionTest();

		bool getIsProxySet() const
		{
			return mIsProxySet;
		}


		const QString& getProxyHostName() const
		{
			return mProxyHostName;
		}


		const QString& getProxyPort() const
		{
			return mProxyPort;
		}


		const QString& getProxyType() const
		{
			return mProxyType;
		}


		const QString& getProxyCapabilities() const
		{
			return mProxyCapabilities;
		}


		bool getConnectionTestWithProxySuccessful() const
		{
			return mConnectionTestWithProxySuccessful;
		}


		bool getConnectionTestWithoutProxySuccessful() const
		{
			return mConnectionTestWithoutProxySuccessful;
		}


		bool getPingTestOnProxySuccessful() const
		{
			return mPingTestOnProxySuccessful;
		}


	Q_SIGNALS:
		void fireConnectionTestDone();
};

} // namespace governikus
