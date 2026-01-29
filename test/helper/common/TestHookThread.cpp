/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#include "TestHookThread.h"

#include "QtHooks.h"

#include <QtEnvironmentVariables>


using namespace governikus;


TestHookThread::TestHookThread(const QString& pThreadName)
	: mCounterExpected(0)
	, mCounterDestroyed(0)
	, mThread()
{
	const bool useXml = QCoreApplication::arguments().contains(QLatin1StringView("-xml"))
			|| QCoreApplication::arguments().contains(QLatin1StringView("-,xml"));
	QtHooks::init(useXml);

	if (qEnvironmentVariableIsSet("AUSWEISAPP_HOOK_PRINT"))
	{
		QtHooks::setPrint();
	}
	mThread.reset(new QThread());
	if (!pThreadName.isEmpty())
	{
		mThread->setObjectName(pThreadName);
	}
	mThread->start();
}


TestHookThread::~TestHookThread()
{
	QElapsedTimer timer;
	timer.start();
	while (mCounterExpected != mCounterDestroyed && timer.elapsed() < 5000)
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
	}

	mThread->quit();
	mThread->wait();
	mThread.reset();

	if (QtHooks::getQObjects().empty())
	{
		QtHooks::clear();
		return;
	}

	QtHooks::printAlive();
	Q_ASSERT(false);
}


void TestHookThread::waitForDestruction(const QObject* pObject)
{
	++mCounterExpected;
	QObject::connect(pObject, &QObject::destroyed, pObject, [this](){
				++mCounterDestroyed;
			});
}
