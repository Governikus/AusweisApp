/*!
 * \brief Provides an UDP socket to send and receive datagrams.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DatagramHandler.h"
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

	private:
		friend class ::test_DatagramHandlerImpl;
		friend struct QtSharedPointer::CustomDeleter<DatagramHandlerImpl, QtSharedPointer::NormalDeleter>;

		QScopedPointer<QUdpSocket, QScopedPointerDeleteLater> mSocket;
		QScopedPointer<MulticastLock> mMulticastLock;
		quint16 mUsedPort;
		PortFile mPortFile;
		bool mEnableListening;

		void resetSocket();
		bool sendToAddress(const QByteArray& pData, const QHostAddress& pAddress, quint16 pPort = 0);
		bool sendToAllAddressEntries(const QByteArray& pData, quint16 pPort);

#if defined(Q_OS_IOS)

		void checkNetworkPermission();
#endif

	public:
		static quint16 cPort;

		DatagramHandlerImpl(bool pEnableListening = true, quint16 pPort = DatagramHandlerImpl::cPort);
		~DatagramHandlerImpl() override;

		[[nodiscard]] bool isBound() const override;
		bool send(const QByteArray& pData) override;

	private Q_SLOTS:
		void onReadyRead();
};


} // namespace governikus
