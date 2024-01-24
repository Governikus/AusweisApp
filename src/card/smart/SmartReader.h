/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of Reader for Smart-eID card.
 */

#pragma once

#include "Reader.h"
#include "SmartCard.h"


class test_SmartReader;


namespace governikus
{

class SmartReader
	: public ConnectableReader
{
	Q_OBJECT
	friend class ::test_SmartReader;

	private:
		QScopedPointer<SmartCard, QScopedPointerDeleteLater> mCard;

	public:
		explicit SmartReader(const QString& pName);

		[[nodiscard]] Card* getCard() const override;
		void connectReader() override;
		void disconnectReader(const QString& pError) override;
};


} // namespace governikus
