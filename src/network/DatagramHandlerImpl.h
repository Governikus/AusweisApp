/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DatagramHandler.h"
#include "HttpServer.h"
#include "MulticastLock.h"
#include "PortFile.h"

#include <QHostAddress>
#include <QList>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QScopedPointer>
#include <QUdpSocket>


namespace governikus
{

class DatagramHandlerImpl
	: public DatagramHandler
{
	Q_OBJECT
	friend class ::test_DatagramHandlerImpl;
	friend struct QtSharedPointer::CustomDeleter<DatagramHandlerImpl, QtSharedPointer::NormalDeleter>;

	private:
		QScopedPointer<QUdpSocket, QScopedPointerDeleteLater> mSocket;
		QScopedPointer<MulticastLock> mMulticastLock;
		QList<QNetworkAddressEntry> mAllEntries;
		QList<QHostAddress> mFailedAddresses;
		quint16 mUsedPort;
		PortFile mPortFile;
		bool mEnableListening;

		void resetSocket();
		[[nodiscard]] bool isValidBroadcastInterface(const QNetworkInterface& pInterface) const;
		[[nodiscard]] bool isValidAddressEntry(const QNetworkAddressEntry& pEntry) const;
		[[nodiscard]] QHostAddress getBroadcastAddress(const QNetworkAddressEntry& pEntry) const;
		[[nodiscard]] bool sendToAddress(const QByteArray& pData, const QHostAddress& pAddress, quint16 pPort = 0, bool pLogError = true);
		void sendToAddressEntries(const QByteArray& pData, const QList<QNetworkAddressEntry>& pEntries, quint16 pPort);

#if defined(Q_OS_IOS)

		void checkNetworkPermission();
#endif

	public:
		DatagramHandlerImpl(bool pEnableListening = true, quint16 pPort = HttpServer::cPort);
		~DatagramHandlerImpl() override;

		[[nodiscard]] bool isBound() const override;
		[[nodiscard]] QList<QNetworkAddressEntry> getAllBroadcastEntries() const override;
		void send(const QByteArray& pData, const QList<QNetworkAddressEntry>& pEntries) override;

	private Q_SLOTS:
		void onReadyRead();
};


} // namespace governikus
