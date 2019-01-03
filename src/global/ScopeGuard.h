/*!
 * \brief Calls a lambda once the object is destroyed
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <functional>

class test_ScopeGuard;

namespace governikus
{

class ScopeGuard
{
	private:
		friend class ::test_ScopeGuard;

		const std::function<void()> mFunction;
		bool mEnabled;

	public:
		ScopeGuard(const ScopeGuard& pCopy) = delete;
		ScopeGuard(const ScopeGuard&& pCopy) = delete;
		ScopeGuard& operator=(const ScopeGuard& pCopy) = delete;
		ScopeGuard& operator=(const ScopeGuard&& pCopy) = delete;

		ScopeGuard(const std::function<void()>& pFunc, bool pEnabled = true);
		~ScopeGuard();

		bool isEnabled() const;
		void setEnabled(bool pEnabled = true);
};

} // namespace governikus
