/*!
 * DataChannel.h
 *
 * \brief Interface modelling a component that can send and receive data blocks in the form
 * of QByteArray objects.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "GlobalStatus.h"

#include <QByteArray>
#include <QObject>

namespace governikus
{
class DataChannel
	: public QObject
{
	Q_OBJECT

	public:
		DataChannel();
		virtual ~DataChannel();

		Q_INVOKABLE virtual void send(const QByteArray& pDataBlock) = 0;
		Q_INVOKABLE virtual void close() = 0;

	Q_SIGNALS:
		void fireReceived(const QByteArray& pDataBlock);
		void fireClosed(GlobalStatus::Code pCloseCode);
};

} /* namespace governikus */
