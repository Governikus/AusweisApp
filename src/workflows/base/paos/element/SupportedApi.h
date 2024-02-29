/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief XML element for "SupportedAPI". See TR-03112-7.
 */

#pragma once

#include <QString>

namespace governikus
{

class SupportedAPI
{
	public:
		[[nodiscard]] QString getMajor() const;
		[[nodiscard]] QString getMinor() const;
		[[nodiscard]] QString getSubminor() const;
};

} // namespace governikus
