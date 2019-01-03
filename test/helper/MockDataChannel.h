/*!
 * \brief Data channel mock for tests.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DataChannel.h"

#include <QByteArray>
#include <QVector>

namespace governikus
{

class MockDataChannel
	: public DataChannel
{
	Q_OBJECT

	private:
		QString mId;
		QVector<QByteArray> mReceivedDataBlocks;

	public:
		MockDataChannel();
		virtual ~MockDataChannel() override;

		virtual void send(const QByteArray& pDataBlock) override;
		virtual void close() override;
		virtual const QString& getId() const override;
		void closeAbnormal();

		const QVector<QByteArray>& getReceivedDataBlocks() const;

	public Q_SLOTS:
		void onReceived(const QByteArray& pDataBlock);

	Q_SIGNALS:
		void fireSend(const QByteArray& pDataBlock);
};


} // namespace governikus
