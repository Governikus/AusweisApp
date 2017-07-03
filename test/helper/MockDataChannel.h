/*!
 * \brief Data channel mock for tests.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "DataChannel.h"

namespace governikus
{

class MockDataChannel
	: public DataChannel
{
	Q_OBJECT

	public:
		MockDataChannel();
		virtual ~MockDataChannel();

		virtual void send(const QByteArray& pDataBlock) override;
		virtual void close() override;
		void closeAbnormal();

	public Q_SLOTS:
		void onReceived(const QByteArray& pDataBlock);

	Q_SIGNALS:
		void fireSend(const QByteArray& pDataBlock);
};


} /* namespace governikus */
