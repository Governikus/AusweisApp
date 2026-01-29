/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include <QObject>
#include <QScopedPointer>
#include <QtTest>


namespace governikus
{

class TestHookThread
{
	private:
		uint mCounterExpected;
		uint mCounterDestroyed;
		QScopedPointer<QThread> mThread;

	public:
		explicit TestHookThread(const QString& pThreadName = QString());
		~TestHookThread();


		QThread* getThread() const
		{
			return mThread.data();
		}


		void waitForDestruction(const QObject* pObject);


};

} // namespace governikus
