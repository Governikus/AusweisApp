/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
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

static bool cPrintXml = false;
static bool cPrintAdd = false;
static bool cPrintRemove = false;

// previously installed Qt hooks, for daisy-chaining
static void (* aa_next_addObject)(QObject*) = nullptr;
static void (* aa_next_removeObject)(QObject*) = nullptr;


extern "C" Q_DECL_EXPORT void aa_addObject(QObject* pObj)
{
	const std::lock_guard locker(cMutex);

	cAllQObjects.push_back(pObj);

	if (cPrintAdd)
	{
		QtHooks::print(pObj, " CREATE ", "qwarn");

		// use QCoreApplication instead of QObject directly: QTBUG-84385
		if (QCoreApplication::instance())
		{
			const QPointer<QObject> ptr = pObj;
			QMetaObject::invokeMethod(QCoreApplication::instance(), [ptr] {
						if (ptr)
						{
							const std::lock_guard lock(cMutex);
							QtHooks::print(ptr, " UPDATE ");
						}
					}, Qt::QueuedConnection);
		}
	}

	if (aa_next_addObject)
	{
		aa_next_addObject(pObj);
	}
}


extern "C" Q_DECL_EXPORT void aa_removeObject(QObject* pObj)
{
	const std::lock_guard locker(cMutex);

	cAllQObjects.remove(pObj);

	if (cPrintRemove)
	{
		QtHooks::print(pObj, " DELETE ");
	}

	if (aa_next_removeObject)
	{
		aa_next_removeObject(pObj);
	}
}


bool QtHooks::init(bool pPrintXml)
{
	cPrintXml = pPrintXml;

#if __has_include(<private/qhooks_p.h>)
	aa_next_addObject = reinterpret_cast<QHooks::AddQObjectCallback>(qtHookData[QHooks::AddQObject]);
	aa_next_removeObject = reinterpret_cast<QHooks::RemoveQObjectCallback>(qtHookData[QHooks::RemoveQObject]);

	qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(&aa_addObject);
	qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(&aa_removeObject);
	return true;

#else
	std::cout << "Cannot register hooks: <private/qhooks_p.h> is not available" << std::endl;
	return false;

#endif
}


void QtHooks::clear()
{
#if __has_include(<private/qhooks_p.h>)
	cAllQObjects.clear();
	qtHookData[QHooks::AddQObject] = 0;
	qtHookData[QHooks::RemoveQObject] = 0;
#endif
}


void QtHooks::setPrint(bool pAdd, bool pRemove)
{
	const std::lock_guard locker(cMutex);

	cPrintAdd = pAdd;
	cPrintRemove = pRemove;
}


void QtHooks::print(const QObject* pObj, const char* pPrefix, const char* pType)
{
	assert(pObj);

	if (cPrintXml)
	{
		std::cout << "  <Message type=\"" << pType << "\" file=\"\" line=\"0\">" << std::endl;
		std::cout << "    <Description><![CDATA[";
	}

	std::cout << "QtHooks:" << pPrefix << pObj->metaObject()->className() << "(" << pObj << "): " << qPrintable(pObj->objectName());
	const auto* const parent = pObj->parent();
	if (parent)
	{
		std::cout << std::endl << "    Parent: " << parent->metaObject()->className() << "(" << parent << "): " << qPrintable(parent->objectName());
	}

	if (cPrintXml)
	{
		std::cout << "]]></Description>" << std::endl;
		std::cout << "  </Message>";
	}
	std::cout << std::endl;
}


void QtHooks::printAlive()
{
	const std::lock_guard locker(cMutex);

	for (const auto* obj : cAllQObjects)
	{
		print(obj, " ALIVE! ", "qfatal");
	}
}


const std::list<QObject*>& QtHooks::getQObjects()
{
	return cAllQObjects;
}
