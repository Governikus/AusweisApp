/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

#ifdef Q_OS_WIN
	#include <QSettings>
	#include <QSharedPointer>
#endif

class test_GeneralSettings;

namespace governikus
{

class AutoStart
{
	friend class ::test_GeneralSettings;

	private:
#ifdef Q_OS_WIN
		[[nodiscard]] static QString appPath();
		[[nodiscard]] static QSharedPointer<QSettings> getRegistryStore();
#endif
		[[nodiscard]] static bool setInternal(bool pEnabled);

	public:
		[[nodiscard]] static bool enabled();
		[[nodiscard]] static bool isSetByAdmin();
		[[nodiscard]] static bool set(bool pEnabled);
#ifdef Q_OS_WIN
		[[nodiscard]] static bool removeOldAutostart();
#endif
};


} // namespace governikus
