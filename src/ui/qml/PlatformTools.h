/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

class PlatformTools
{
	public:
		static void hideFromTaskbar();
		static void restoreToTaskbar();
		static void postNotification(const QString& pTitle, const QString& pMessage);
};

} // namespace governikus
