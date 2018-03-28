/*!
 * \brief Helper for some miscellaneous methods.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

namespace governikus
{

class RemoteHelper
{
	private:
		RemoteHelper() = delete;

	public:
		static bool checkAndGenerateKey();
};


} /* namespace governikus */
