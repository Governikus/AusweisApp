/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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
		static QString appPath();
		static QSharedPointer<QSettings> getRegistryStore();
#endif
		static bool setInternal(bool pEnabled);

	public:
		static bool enabled();
		static bool isSetByAdmin();
		static bool set(bool pEnabled);
};


} // namespace governikus
