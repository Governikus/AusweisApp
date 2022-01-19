/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApduBuilder.h"

class test_Commands;

namespace governikus
{

class EABuilder
	: public CommandApduBuilder
{
	public:
		EABuilder();
		void setSignature(const QByteArray& pData);
		CommandApdu build() override;

	private:
		friend class ::test_Commands;
		QByteArray mSignature;
};

} // namespace governikus
