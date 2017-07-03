/*!
 * \brief Provides an interface to send and receive datagrams over UDP.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include <QHostAddress>
#include <QJsonDocument>
#include <QSharedPointer>


namespace governikus
{


class DatagramHandler
	: public QObject
{
	Q_OBJECT

	public:
		static void registerMetaTypes();

		DatagramHandler() = default;
		virtual ~DatagramHandler();
		virtual bool isBound() const = 0;
		virtual bool send(const QJsonDocument& pData, const QHostAddress& pAddress = QHostAddress::Broadcast) = 0;

	Q_SIGNALS:
		void fireNewMessage(const QJsonDocument& pData, const QHostAddress& pAddress);
};


} /* namespace governikus */
