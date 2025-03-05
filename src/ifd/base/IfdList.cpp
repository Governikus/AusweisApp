/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "IfdListImpl.h"

#include <QtGlobal>

using namespace governikus;


namespace governikus
{
template<> IfdList* createNewObject<IfdList*>()
{
	return new IfdListImpl();
}


template<> IfdList* createNewObject<IfdList*, int, int>(int&& pCheckInterval, int&& pTimeout)
{
	return new IfdListImpl(pCheckInterval, pTimeout);
}


} // namespace governikus


QList<QSharedPointer<IfdListEntry>> IfdList::getIfdList() const
{
	return QList<QSharedPointer<IfdListEntry>>();
}
