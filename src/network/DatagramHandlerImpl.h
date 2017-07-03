/*!
 * \brief Provides an UDP socket to send and receive datagrams.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "DatagramHandler.h"

#include <QScopedPointer>
#include <QSharedPointer>
#include <QUdpSocket>

class test_DatagramHandlerImpl;

namespace governikus
{

class DatagramHandlerImpl
	: public DatagramHandler
{
	Q_OBJECT

	private:
		friend class::test_DatagramHandlerImpl;
		friend struct QtSharedPointer::CustomDeleter<DatagramHandlerImpl, QtSharedPointer::NormalDeleter>;

		static quint16 cPort;
		QScopedPointer<QUdpSocket, QScopedPointerDeleteLater> mSocket;

	public:
		DatagramHandlerImpl();
		virtual ~DatagramHandlerImpl();

		virtual bool isBound() const override;
		virtual bool send(const QJsonDocument& pData, const QHostAddress& pAddress = QHostAddress::Broadcast) override;

	private Q_SLOTS:
		void onReadyRead();
};


} /* namespace governikus */
