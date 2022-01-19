/*!
 * \brief Parser for the PAOS Transmit element.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QXmlStreamReader>

#include "paos/retrieve/PaosParser.h"
#include "paos/retrieve/Transmit.h"

namespace governikus
{

class TransmitParser
	: public PaosParser
{
	public:
		TransmitParser();
		~TransmitParser() override;

	protected:
		PaosMessage* parseMessage() override;

	private:
		void parseSlotHandle();
		void parseInputApduInfo();

	private:
		QScopedPointer<Transmit> mTransmit;
};

} // namespace governikus
