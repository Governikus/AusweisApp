/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"

#include "SingletonHelper.h"

using namespace governikus;

defineSingleton(Env)

Env::Env()
#ifndef QT_NO_DEBUG
	: mLock(QReadWriteLock::Recursive)
#endif
{
}


Env& Env::getInstance()
{
	return *Instance;
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

	const QWriteLocker locker(&holder.mLock);
	holder.mInstancesSingleton.clear();
	holder.mInstancesCreator.clear();

	const QWriteLocker lockerShared(&holder.mSharedInstancesLock);
	holder.mSharedInstances.clear();
}


void Env::set(const QMetaObject& pMetaObject, void* pObject)
{
	const Identifier id = pMetaObject.className();
	Q_ASSERT_X(!QByteArray(id).toLower().contains("mock"), "test", "Do you really want to mock a mock?");

	auto& holder = getInstance();
	const QWriteLocker locker(&holder.mLock);

	if (pObject)
	{
		qDebug() << "Add mock:" << id;
		holder.mInstancesSingleton.insert(id, pObject);
	}
	else
	{
		qDebug() << "Remove mock:" << id;
		holder.mInstancesSingleton.remove(id);
	}
}


void Env::setShared(const QMetaObject& pMetaObject, QSharedPointer<QObject> pObject)
{
	const Identifier className = pMetaObject.className();
	auto& holder = getInstance();
	const QWriteLocker locker(&holder.mSharedInstancesLock);

	if (pObject)
	{
		qDebug() << "Add shared mock:" << className;
		holder.mSharedInstances.insert(className, pObject.toWeakRef());
	}
	else
	{
		qDebug() << "Remove shared mock:" << className;
		holder.mSharedInstances.remove(className);
	}
}


Env::FuncWrapperBase::~FuncWrapperBase()
{
}


#endif
