/*!
 * \brief Interface modelling a component that can send and receive data blocks in the form
 * of QByteArray objects.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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
		DataChannel() = default;
		~DataChannel() override;

		Q_INVOKABLE virtual void send(const QByteArray& pDataBlock) = 0;
		Q_INVOKABLE virtual void close() = 0;
		[[nodiscard]] virtual const QString& getId() const = 0;

	Q_SIGNALS:
		void fireReceived(const QByteArray& pDataBlock);
		void fireClosed(GlobalStatus::Code pCloseCode);
};

} // namespace governikus
