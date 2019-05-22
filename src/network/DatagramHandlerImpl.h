/*!
 * \brief Provides an UDP socket to send and receive datagrams.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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

		bool send(const QByteArray& pData, const QHostAddress& pAddress, quint16 pPort = 0);
		bool send(const QByteArray& pData, quint16 pPort);

	public:
		static quint16 cPort;

		DatagramHandlerImpl(bool pListen = true, quint16 pPort = DatagramHandlerImpl::cPort);
		virtual ~DatagramHandlerImpl() override;

		virtual bool isBound() const override;
		virtual bool send(const QByteArray& pData) override;

	private Q_SLOTS:
		void onReadyRead();
};


} // namespace governikus
