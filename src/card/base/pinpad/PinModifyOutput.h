/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"
#include "apdu/ResponseApdu.h"

#include <QByteArray>


namespace governikus
{

class PinModifyOutput
{
	private:
		ResponseApdu mResponseApdu;

	public:
		PinModifyOutput();
		explicit PinModifyOutput(const ResponseApdu& pResponseApdu);

		[[nodiscard]] const ResponseApdu& getResponseApdu() const;

		/**
		 * Defined in TR-03119 Section D.3 and DWG_Smart-Card_CCID_Rev110
		 * Section 6.2.1
		 */
		[[nodiscard]] QByteArray toCcid() const;

};

} // namespace governikus
