/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

namespace governikus
{

class MulticastLock
{
	private:
		void invokeJniMethod(const char* const pMethodName) const;

	public:
		MulticastLock();
		~MulticastLock();
};


} // namespace governikus
