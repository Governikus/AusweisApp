/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
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
