/*!
 * \brief Provides an UDP socket to send and receive datagrams.
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
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
		friend class ::test_DatagramHandlerImpl;
		friend struct QtSharedPointer::CustomDeleter<DatagramHandlerImpl, QtSharedPointer::NormalDeleter>;

		static quint16 cPort;
		QScopedPointer<QUdpSocket, QScopedPointerDeleteLater> mSocket;

		bool send(const QJsonDocument& pData, const QHostAddress& pAddress);

	public:
		DatagramHandlerImpl(bool pListen = true);
		virtual ~DatagramHandlerImpl() override;

		virtual bool isBound() const override;
		virtual bool send(const QJsonDocument& pData) override;

	private Q_SLOTS:
		void onReadyRead();
};


} /* namespace governikus */
