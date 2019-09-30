/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApduBuilder.h"

namespace governikus
{

class GetChallengeBuilder
	: public CommandApduBuilder
{
	public:
		GetChallengeBuilder();
		CommandApdu build() override;
};

} // namespace governikus
