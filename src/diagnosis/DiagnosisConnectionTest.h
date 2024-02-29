/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class for retrieving information about the system proxy, pinging the proxy, trying
 * to establish a connection to a test server with and without the proxy and providing the
 * results to the DiagnosisModel.
 */

#pragma once

#include <QNetworkProxy>
#include <QNetworkReply>
#include <QObject>
#include <QTcpSocket>

class test_DiagnosisConnectionTest;
class test_DiagnosisModel;
class test_DiagnosisTreeModel;

namespace governikus
{

class DiagnosisConnectionTest
	: public QObject
{
	Q_OBJECT
	friend class ::test_DiagnosisConnectionTest;
	friend class ::test_DiagnosisModel;
	friend class ::test_DiagnosisTreeModel;

	private:
		bool mIsProxySet;
		QString mProxyHostName;
		QString mProxyPort;
		QString mProxyType;
		QString mProxyCapabilities;

		QString mErrorOfConnectionTestWithProxy;
		bool mConnectionTestWithProxySuccessful;
		bool mConnectionTestWithoutProxySuccessful;

		bool mPingTestOnProxySuccessful;

		bool mProxyPingDone;
		bool mConnectionTestWithProxyDone;
		bool mConnectionTestWithoutProxyDone;

		QTcpSocket mPingSocketToProxy;
		QTcpSocket mTcpSocketWithoutProxy;
		QSharedPointer<QNetworkReply> mReplyWithProxy;

		void checkIfAllProcessesDone();
		static QString getProxyTypeAsQString(QNetworkProxy::ProxyType pType);
		static QString getProxyCapabilitiesAsQString(QNetworkProxy::Capabilities pCaps);

	private Q_SLOTS:
		void onProxyPingTestDone();
		void onProxyPingTestError(QAbstractSocket::SocketError pSocketError);
		void onSocketConnectionTestWithProxyDone();
		void onSocketConnectionTestWithoutProxyDone();
		void onSocketConnectionTestWithoutProxyError(QAbstractSocket::SocketError pSocketError);

	public:
		DiagnosisConnectionTest();
		void startConnectionTest();

		[[nodiscard]] bool getIsProxySet() const
		{
			return mIsProxySet;
		}


		[[nodiscard]] const QString& getProxyHostName() const
		{
			return mProxyHostName;
		}


		[[nodiscard]] const QString& getProxyPort() const
		{
			return mProxyPort;
		}


		[[nodiscard]] const QString& getProxyType() const
		{
			return mProxyType;
		}


		[[nodiscard]] const QString& getProxyCapabilities() const
		{
			return mProxyCapabilities;
		}


		[[nodiscard]] bool getConnectionTestWithProxySuccessful() const
		{
			return mConnectionTestWithProxySuccessful;
		}


		[[nodiscard]] const QString& getErrorOfConnectionTestWithProxy() const
		{
			return mErrorOfConnectionTestWithProxy;
		}


		[[nodiscard]] bool getConnectionTestWithoutProxySuccessful() const
		{
			return mConnectionTestWithoutProxySuccessful;
		}


		[[nodiscard]] bool getPingTestOnProxySuccessful() const
		{
			return mPingTestOnProxySuccessful;
		}

	Q_SIGNALS:
		void fireConnectionTestDone();
};

} // namespace governikus
