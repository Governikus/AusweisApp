/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Provides an UDP socket to send and receive datagrams.
 */

#pragma once

#include "DatagramHandler.h"
#include "HttpServer.h"
#include "MulticastLock.h"
#include "PortFile.h"

#include <QScopedPointer>
#include <QSharedPointer>
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
		QList<QHostAddress> mAllAddresses;
		QVector<QHostAddress> mFailedAddresses;
		quint16 mUsedPort;
		PortFile mPortFile;
		bool mEnableListening;

		void resetSocket();
		[[nodiscard]] bool isValidBroadcastInterface(const QNetworkInterface& pInterface) const;
		[[nodiscard]] QVector<QHostAddress> getAllBroadcastAddresses(const QNetworkInterface& pInterface) const;
		[[nodiscard]] bool sendToAddress(const QByteArray& pData, const QHostAddress& pAddress, quint16 pPort = 0, bool pLogError = true);
		void sendToAllAddressEntries(const QByteArray& pData, quint16 pPort);

#if defined(Q_OS_IOS)

		void checkNetworkPermission();
#endif

	public:
		DatagramHandlerImpl(bool pEnableListening = true, quint16 pPort = HttpServer::cPort);
		~DatagramHandlerImpl() override;

		[[nodiscard]] bool isBound() const override;
		void send(const QByteArray& pData) override;

	private Q_SLOTS:
		void onReadyRead();
};


} // namespace governikus
