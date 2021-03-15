/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApduBuilder.h"

class test_Commands;

namespace governikus
{

class ResetRetryCounterBuilder
	: public CommandApduBuilder
{
	public:
		explicit ResetRetryCounterBuilder(const QByteArray& pPin = QByteArray());
		CommandApdu build() override;

	private:
		QByteArray mPin;
};

} // namespace governikus
