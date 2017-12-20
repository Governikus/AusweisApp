/*!
 * \brief Provides an interface to send and receive datagrams over UDP.
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QHostAddress>
#include <QJsonDocument>
#include <QSharedPointer>

class test_DatagramHandlerImpl;

namespace governikus
{

class DatagramHandler
	: public QObject
{
	Q_OBJECT

	private:
		friend class ::test_DatagramHandlerImpl;

	public:
		DatagramHandler(bool pListen = true);
		virtual ~DatagramHandler();
		virtual bool isBound() const = 0;
		virtual bool send(const QJsonDocument& pData) = 0;

	Q_SIGNALS:
		void fireNewMessage(const QJsonDocument& pData, const QHostAddress& pAddress);
};


} /* namespace governikus */
