/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QObject>

namespace governikus
{

defineEnumType(Platform,
		ANDROID,
		IOS,
		MOBILE,
		DESKTOP)

/// Determine platform at runtime. Use this instead of the preprocessor to allow running the mobile UI on the desktop.
bool isPlatform(const QObject&, Platform selector);

} // namespace governikus
