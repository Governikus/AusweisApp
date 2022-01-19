/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApdu.h"

#include <QObject>

namespace governikus
{

class CommandApduBuilder
{
	private:
		Q_DISABLE_COPY(CommandApduBuilder)

	public:
		CommandApduBuilder();
		virtual ~CommandApduBuilder();
		virtual CommandApdu build() = 0;
};

} // namespace governikus
