/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once


namespace governikus
{


class AutoStart
{
	public:
		static bool enabled();
		static void set(bool pEnabled);
};


} /* namespace governikus */
