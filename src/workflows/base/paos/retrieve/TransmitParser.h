/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Parser for the PAOS Transmit element.
 */

#pragma once

#include "paos/retrieve/PaosParser.h"
#include "paos/retrieve/Transmit.h"

#include <QXmlStreamReader>

#include <memory>

namespace governikus
{

class TransmitParser
	: public PaosParser
{
	public:
		TransmitParser();
		~TransmitParser() override = default;

	protected:
		PaosMessage* parseMessage() override;

	private:
		void parseSlotHandle() const;
		void parseInputApduInfo();

	private:
		std::unique_ptr<Transmit> mTransmit;
};

} // namespace governikus
