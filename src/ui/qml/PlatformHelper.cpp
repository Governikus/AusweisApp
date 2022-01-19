/*!
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "PlatformHelper.h"

#include <QFileSelector>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlFileSelector>

namespace governikus
{

bool isPlatform(const QObject& object, const Platform selector)
{
	if (const QQmlContext* const context = QQmlEngine::contextForObject(&object))
	{
		// The UI could have been changed using OVERRIDE_PLATFORM_SELECTOR
		if (QQmlEngine* const engine = context->engine())
		{
			const QStringList selectors = QQmlFileSelector::get(engine)->selector()->extraSelectors();
			switch (selector)
			{
				case Platform::ANDROID:
					return selectors.contains(QLatin1String("android"));

				case Platform::IOS:
					return selectors.contains(QLatin1String("ios"));

				case Platform::MOBILE:
					return selectors.contains(QLatin1String("mobile"));

				case Platform::DESKTOP:
					return selectors.contains(QLatin1String("desktop"));
			}
		}
	}
#ifdef Q_OS_IOS
	if (selector == Platform::MOBILE || selector == Platform::IOS)
	{
		return true;
	}
#endif
#ifdef Q_OS_ANDROID
	if (selector == Platform::MOBILE || selector == Platform::ANDROID)
	{
		return true;
	}
#endif
#if ((!defined(Q_OS_ANDROID) && defined(Q_OS_LINUX)) || defined(Q_OS_WIN) || defined(Q_OS_MACOS) || defined(Q_OS_FREEBSD))
	if (selector == Platform::DESKTOP)
	{
		return true;
	}
#endif
	return false;
}


} // namespace governikus
