/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Data channel mock for tests.
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
		~MockDataChannel() override;

		void send(const QByteArray& pDataBlock) override;
		void close() override;
		[[nodiscard]] const QString& getId() const override;
		void closeAbnormal();

		[[nodiscard]] const QVector<QByteArray>& getReceivedDataBlocks() const;

	public Q_SLOTS:
		void onReceived(const QByteArray& pDataBlock);

	Q_SIGNALS:
		void fireSend(const QByteArray& pDataBlock);
};


} // namespace governikus
