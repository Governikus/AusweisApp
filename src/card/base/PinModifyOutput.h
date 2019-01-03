/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"
#include "ResponseApdu.h"

#include <QByteArray>


namespace governikus
{

class PinModifyOutput
{
	private:
		ResponseApdu mResponseApdu;

		static CardReturnCode statusCodeToCardReturnCode(const QByteArray& pData);

	public:
		PinModifyOutput();
		PinModifyOutput(const ResponseApdu& pResponseApdu);

		CardReturnCode getReturnCode() const;
		const ResponseApdu& getResponseApdu() const;

		/**
		 * Defined in TR-03119 Section D.3 and DWG_Smart-Card_CCID_Rev110
		 * Section 6.2.1
		 */
		QByteArray toCcid() const;

};

} // namespace governikus
