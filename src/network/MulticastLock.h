/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

namespace governikus
{

class MulticastLock
{
	private:
		void invokeJniMethod(const char* pMethodName);

	public:
		MulticastLock();
		~MulticastLock();
};


} // namespace governikus
