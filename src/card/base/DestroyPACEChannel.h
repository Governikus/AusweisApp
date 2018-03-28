/*!
 * \brief Data object for creation of card command DestroyPACEChannel
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Apdu.h"

#include <QByteArray>


namespace governikus
{


class DestroyPACEChannelBuilder
{
	public:
		/**
		 * Defined in pcsc10_v2.02.08_amd1.1
		 */
		QByteArray createCommandData();

		/**
		 * Defined in BSI-TR-03119_V1_pdf
		 */
		CommandApdu createCommandDataCcid();
};


}  // namespace governikus
