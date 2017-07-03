/*
 * \brief Helper to get build date and time.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

namespace governikus
{

class BuildHelper
{
	private:
		static const char* mDateTime;
		BuildHelper() = delete;
		~BuildHelper() = delete;

	public:
		static const char* getDateTime()
		{
			return mDateTime;
		}


};

} /* namespace governikus */
