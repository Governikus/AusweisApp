/*!
 * \brief XML element for "SupportedAPI". See TR-03112-7.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

class SupportedAPI
{
	public:
		QString getMajor() const;
		QString getMinor() const;
		QString getSubminor() const;
};

} /* namespace governikus */
