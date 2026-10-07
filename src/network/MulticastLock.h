/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
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
