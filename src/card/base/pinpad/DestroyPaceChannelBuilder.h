/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "apdu/CommandApdu.h"

#include <QByteArray>


namespace governikus
{

class DestroyPaceChannelBuilder
{
	public:
		/**
		 * Defined in pcsc10_v2.02.08_amd1.1
		 */
		[[nodiscard]] QByteArray createASN1Struct() const;

		/**
		 * Defined in BSI-TR-03119_V1_pdf
		 */
		[[nodiscard]] CommandApdu createASN1StructCcid() const;
};


}  // namespace governikus
