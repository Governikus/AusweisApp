/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#include "QtHooks.h"

#if __has_include(<private/qhooks_p.h>)
#include <private/qhooks_p.h>
#endif

#include <QCoreApplication>
#include <QMetaObject>
#include <QPointer>

#include <iostream>
#include <mutex>

using namespace governikus;

static std::list<QObject*> cAllQObjects = {}; // clazy:exclude=non-pod-global-static
static std::mutex cMutex = {};

static bool cPrintAdd = false;
static bool cPrintRemove = false;

// previously installed Qt hooks, for daisy-chaining
static void (* aa2_next_addObject)(QObject*) = nullptr;
static void (* aa2_next_removeObject)(QObject*) = nullptr;


extern "C" Q_DECL_EXPORT void aa2_addObject(QObject* pObj)
{
	const std::lock_guard locker(cMutex);

	cAllQObjects.push_back(pObj);

	if (cPrintAdd)
	{
		QtHooks::print(pObj, " > ");

		// use QCoreApplication instead of QObject directly: QTBUG-84385
		if (QCoreApplication::instance())
		{
			const QPointer<QObject> ptr = pObj;
			QMetaObject::invokeMethod(QCoreApplication::instance(), [ptr] {
					if (ptr)
					{
						const std::lock_guard lock(cMutex);
						QtHooks::print(ptr, " ~ ");
					}
				}, Qt::QueuedConnection);
		}
	}

	if (aa2_next_addObject)
	{
		aa2_next_addObject(pObj);
	}
}


extern "C" Q_DECL_EXPORT void aa2_removeObject(QObject* pObj)
{
	const std::lock_guard locker(cMutex);

	cAllQObjects.remove(pObj);

	if (cPrintRemove)
	{
		QtHooks::print(pObj, " < ");
	}

	if (aa2_next_removeObject)
	{
		aa2_next_removeObject(pObj);
	}
}


bool QtHooks::init()
{
#if __has_include(<private/qhooks_p.h>)
	aa2_next_addObject = reinterpret_cast<QHooks::AddQObjectCallback>(qtHookData[QHooks::AddQObject]);
	aa2_next_removeObject = reinterpret_cast<QHooks::RemoveQObjectCallback>(qtHookData[QHooks::RemoveQObject]);

	qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(&aa2_addObject);
	qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(&aa2_removeObject);
	return true;

#else
	std::cout << "Cannot register hooks: <private/qhooks_p.h> is not available" << std::endl;
	return false;

#endif
}


void QtHooks::setPrint(bool pAdd, bool pRemove)
{
	const std::lock_guard locker(cMutex);

	cPrintAdd = pAdd;
	cPrintRemove = pRemove;
}


void QtHooks::print(const QObject* pObj, const char* pPrefix)
{
	assert(pObj);

	std::cout << pPrefix << pObj->metaObject()->className() << "(" << pObj << ")" << std::endl;
	if (pObj->parent())
	{
		std::cout << "   Parent: " << pObj->parent()->metaObject()->className() << "(" << pObj->parent() << ")" << std::endl;
	}
}


void QtHooks::printAlive()
{
	const std::lock_guard locker(cMutex);

	for (const auto* obj : cAllQObjects)
	{
		print(obj);
	}
}


const std::list<QObject*>& QtHooks::getQObjects()
{
	return cAllQObjects;
}
