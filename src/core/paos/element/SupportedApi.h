/*!
 * \brief XML element for "SupportedAPI".
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
