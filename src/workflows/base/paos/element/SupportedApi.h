/*!
 * \brief XML element for "SupportedAPI". See TR-03112-7.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
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
