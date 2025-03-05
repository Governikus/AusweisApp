/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DataChannel.h"

#include <QByteArray>
#include <QList>

namespace governikus
{

class MockDataChannel
	: public DataChannel
{
	Q_OBJECT

	private:
		bool mPairing;
		QByteArray mId;
		QList<QByteArray> mReceivedDataBlocks;

	public:
		explicit MockDataChannel(bool pPairing = false);
		~MockDataChannel() override;

		void send(const QByteArray& pDataBlock) override;
		void close() override;
		[[nodiscard]] bool isPairingConnection() const override;
		[[nodiscard]] const QByteArray& getId() const override;
		void closeAbnormal();

		[[nodiscard]] const QList<QByteArray>& getReceivedDataBlocks() const;

	public Q_SLOTS:
		void onReceived(const QByteArray& pDataBlock);

	Q_SIGNALS:
		void fireSend(const QByteArray& pDataBlock);
};


} // namespace governikus
