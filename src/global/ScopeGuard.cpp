/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ScopeGuard.h"

#include <QDebug>

using namespace governikus;

ScopeGuard::ScopeGuard(const std::function<void()>& pFunc, bool pEnabled)
	: mFunction(pFunc)
	, mEnabled(pEnabled)
{
	if (!mFunction)
	{
		qWarning() << "Cannot call an empty function";
	}
}


ScopeGuard::~ScopeGuard()
{
	if (mEnabled && mFunction)
	{
		mFunction();
	}
}


bool ScopeGuard::isEnabled() const
{
	return mEnabled;
}


void ScopeGuard::setEnabled(bool pEnabled)
{
	mEnabled = pEnabled;
}
