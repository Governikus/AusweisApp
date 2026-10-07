/**
 * Copyright (c) 2022-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include <QSettings>
#include <QSharedPointer>


namespace governikus
{

class Backup
{
	public:
		static void disable(const QSharedPointer<QSettings>& pSettings);
};


} // namespace governikus
