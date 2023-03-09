/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Provides UserAgent information for PAOS elements.
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
