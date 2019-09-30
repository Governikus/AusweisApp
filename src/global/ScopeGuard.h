/*!
 * \brief Calls a lambda once the object is destroyed
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <functional>
#include <QtGlobal>

class test_ScopeGuard;

namespace governikus
{

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED

class [[deprecated ("Use qScopeGuard")]] ScopeGuard
{
	private:
		friend class ::test_ScopeGuard;
		friend ScopeGuard qScopeGuard(const std::function<void()> &pFunc);

		const std::function<void()> mFunction;
		bool mEnabled;

		ScopeGuard(const std::function<void()> &pFunc, bool pEnabled = true);

		bool isEnabled() const;
		void setEnabled(bool pEnabled = true);

	public:
		ScopeGuard(const ScopeGuard& pCopy) = delete;
		ScopeGuard(const ScopeGuard && pCopy) = delete;
		ScopeGuard& operator=(const ScopeGuard& pCopy) = delete;
		ScopeGuard& operator=(const ScopeGuard&& pCopy) = delete;

		~ScopeGuard();
};

#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
inline ScopeGuard qScopeGuard(const std::function<void()>& pFunc)
{
	return ScopeGuard(pFunc);
}


#endif

QT_WARNING_POP

} // namespace governikus
