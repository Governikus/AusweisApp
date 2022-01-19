/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApduBuilder.h"

class test_Commands;

namespace governikus
{

class ReadBinaryBuilder
	: public CommandApduBuilder
{
	private:
		uint mOffset;
		int mLe;

	public:
		explicit ReadBinaryBuilder(uint pOffset, int pLe);
		CommandApdu build() override;
};

} // namespace governikus
