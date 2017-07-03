/*
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "EnvHolder.h"

#include "SingletonHelper.h"

#include <QDebug>

using namespace governikus;

defineSingleton(EnvHolder)

EnvHolder::EnvHolder()
{
}


EnvHolder::~EnvHolder()
{
}


EnvHolder& EnvHolder::getInstance()
{
	return *Instance;
}


void* EnvHolder::getStoredSingleton(EnvHolder::Identifier pId) const
{
	switch (mTypeInfo.value(pId, Type::UNDEFINED))
	{
		case Type::OWNERSHIP:
			return mInstancesOwnership.value(pId).get();

		case Type::UNMANAGED:
			return mInstancesUnmanaged.value(pId);

		case Type::UNDEFINED:
			return nullptr;
	}

	return nullptr;
}


#ifndef QT_NO_DEBUG

void EnvHolder::clear()
{
	auto& holder = getInstance();
	holder.mInstancesOwnership.clear();
	holder.mInstancesUnmanaged.clear();
	holder.mTypeInfo.clear();
	holder.mInstancesCreator.clear();
	holder.mSharedInstances.clear();
}


void EnvHolder::set(const QMetaObject& pMetaObject, void* pObject, bool pOwnership)
{
	auto className = pMetaObject.className();
	auto& holder = getInstance();
	if (pObject)
	{
		qDebug() << "Add instance:" << className;
		holder.mInstancesOwnership.remove(className);
		holder.mInstancesUnmanaged.insert(className, pObject);
		holder.mTypeInfo.insert(className, pOwnership ? Type::OWNERSHIP : Type::UNMANAGED);
	}
	else
	{
		qDebug() << "Remove instance:" << className;
		holder.mInstancesOwnership.remove(className);
		holder.mInstancesUnmanaged.remove(className);
		holder.mTypeInfo.remove(className);
	}
}


void governikus::EnvHolder::setCreator(const QMetaObject& pMetaObject, const std::function<void*()>& pFunc)
{
	auto className = pMetaObject.className();
	auto& holder = getInstance();
	if (pFunc)
	{
		holder.mInstancesCreator.insert(className, pFunc);
	}
	else
	{
		holder.mInstancesCreator.remove(className);
	}
}


void EnvHolder::setShared(const QMetaObject& pMetaObject, QSharedPointer<QObject> pObject)
{
	auto className = pMetaObject.className();
	auto& holder = getInstance().mSharedInstances;
	if (pObject)
	{
		qDebug() << "Add shared instance:" << className;
		holder.insert(className, pObject.toWeakRef());
	}
	else
	{
		qDebug() << "Remove shared instance:" << className;
		holder.remove(className);
	}
}


#endif
