/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of Reader for Simulator.
 */

#pragma once

#include "Reader.h"
#include "SimulatorCard.h"


namespace governikus
{

class SimulatorReader
	: public ConnectableReader
{
	Q_OBJECT

	private:
		QScopedPointer<SimulatorCard, QScopedPointerDeleteLater> mCard;

	public:
		SimulatorReader();

		[[nodiscard]] Card* getCard() const override;
		void insertCard(const QVariant& pData = QVariant()) override;
		void connectReader() override;
		void disconnectReader(const QString& pError) override;
};


} // namespace governikus
