/*!
 * \brief Contains non-persistent settings.
 *
 * This is not integrated into AppSettings to avoid
 * circular dependency because settings can use this, too.
 *
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"

#include <QObject>

#ifndef QT_NO_QDEBUG
#include <QScopeGuard>

#define SDK_MODE(pEnable)\
	const auto sdkMode = Env::getSingleton<VolatileSettings>()->isUsedAsSDK();\
	Env::getSingleton<VolatileSettings>()->setUsedAsSDK(pEnable);\
	const auto sdkModeGuard = qScopeGuard([sdkMode] {\
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(sdkMode);\
		})

#endif

namespace governikus
{

class VolatileSettings
{
	Q_GADGET

	friend class Env;

	private:
		bool mUsedAsSdk;

	protected:
		VolatileSettings();
		~VolatileSettings() = default;
		static VolatileSettings& getInstance();

	public:
		bool isUsedAsSDK() const;
		void setUsedAsSDK(bool pSdk);
};


} // namespace governikus
