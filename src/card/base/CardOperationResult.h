/*!
 * \brief Generic class representing the result of a card operation, or an error.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"

namespace governikus
{
template<typename T>
class CardOperationResult
{
	private:
		const CardReturnCode mReturnCode;
		const T mPayload;

	public:
		CardOperationResult(const CardReturnCode& pReturnCode, const T& pPayload)
			: mReturnCode(pReturnCode)
			, mPayload(pPayload)
		{
		}


		const CardReturnCode& getReturnCode() const
		{
			return mReturnCode;
		}


		const T& getPayload() const
		{
			return mPayload;
		}


};

} // namespace governikus
