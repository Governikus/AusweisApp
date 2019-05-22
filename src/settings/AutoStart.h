/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once


namespace governikus
{

class AutoStart
{
	public:
		static bool enabled();
		static bool isSetByAdmin();
		static void set(bool pEnabled);
};


} // namespace governikus
