/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QtGlobal>

namespace governikus
{

class DpiCalculator
{
	DpiCalculator()
	{
	}


	public:
		static qreal getDpi();
};

} /* namespace governikus */
