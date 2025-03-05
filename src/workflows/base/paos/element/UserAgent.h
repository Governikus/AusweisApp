/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

class UserAgent
{
	private:
		QString mMajor;
		QString mMinor;
		QString mSubminor;

	public:
		UserAgent();

		[[nodiscard]] QString getName() const;
		[[nodiscard]] const QString& getVersionMajor() const;
		[[nodiscard]] const QString& getVersionMinor() const;
		[[nodiscard]] const QString& getVersionSubminor() const;
};

} // namespace governikus
