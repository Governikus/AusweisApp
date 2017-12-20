/*
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"

#include "SingletonHelper.h"

#include <QDebug>

using namespace governikus;

defineSingleton(Env)

Env::Env()
{
}


Env::~Env()
{
}


Env& Env::getInstance()
{
	return *Instance;
}


void Env::storeSingleton(Identifier pId, void* pObject)
{
	if (pObject)
	{
		qDebug() << "Add instance:" << pId;
		mInstancesOwnership.remove(pId);
		mInstancesUnmanaged.insert(pId, pObject);
		mTypeInfo.insert(pId, Type::UNMANAGED);
	}
	else
	{
		removeStoredSingleton(pId);
	}
}


void Env::storeSingleton(Identifier pId, std::shared_ptr<void> pObject)
{
	if (pObject)
	{
		qDebug() << "Add owned instance:" << pId;
		mInstancesUnmanaged.remove(pId);
		mInstancesOwnership.insert(pId, pObject);
		mTypeInfo.insert(pId, Type::OWNERSHIP);
	}
	else
	{
		removeStoredSingleton(pId);
	}
}


void Env::removeStoredSingleton(Identifier pId)
{
	qDebug() << "Remove instance:" << pId;
	mInstancesOwnership.remove(pId);
	mInstancesUnmanaged.remove(pId);
	mTypeInfo.remove(pId);
}


void* Env::fetchStoredSingleton(Env::Identifier pId) const
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

	Q_UNREACHABLE();
}


#ifndef QT_NO_DEBUG

void Env::resetCounter()
{
	for (auto& mock : qAsConst(getInstance().mInstancesCreator))
	{
		mock->reset();
	}
}


void Env::clear()
{
	auto& holder = getInstance();
	holder.mInstancesOwnership.clear();
	holder.mInstancesUnmanaged.clear();
	holder.mTypeInfo.clear();
	holder.mInstancesCreator.clear();
	holder.mSharedInstances.clear();
}


void Env::set(const QMetaObject& pMetaObject, void* pObject)
{
	Identifier className = pMetaObject.className();
	Q_ASSERT_X(!QByteArray(className).toLower().contains("mock"), "test", "Do you really want to mock a mock?");
	getInstance().storeSingleton(className, pObject);

}


void Env::set(const QMetaObject& pMetaObject, std::shared_ptr<void> pObject)
{
	Identifier className = pMetaObject.className();
	Q_ASSERT_X(!QByteArray(className).toLower().contains("mock"), "test", "Do you really want to mock a mock?");
	getInstance().storeSingleton(className, pObject);
}


void Env::setShared(const QMetaObject& pMetaObject, QSharedPointer<QObject> pObject)
{
	Identifier className = pMetaObject.className();
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


Env::FuncWrapperBase::~FuncWrapperBase()
{
}


#endif
