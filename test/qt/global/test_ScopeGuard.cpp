/*!
 * \brief Unit tests for \ref ScopeGuard
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ScopeGuard.h"

#include "LogHandler.h"

#include <QtTest>

using namespace governikus;

class test_ScopeGuard
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void simple()
		{
			bool flipMe = false;
			{
				const ScopeGuard obj([&flipMe] {
							flipMe = true;
						});

				QVERIFY(obj.mFunction);
				QVERIFY(obj.isEnabled());
				QVERIFY(!flipMe);
			}
			QVERIFY(flipMe);
		}


		void disabled()
		{
			bool flipMe = false;
			{
				const ScopeGuard obj([&flipMe] {
							flipMe = true;
						}, false);

				QVERIFY(obj.mFunction);
				QVERIFY(!obj.isEnabled());
				QVERIFY(!flipMe);
			}
			QVERIFY(!flipMe);
		}


		void disabledAfterCtor()
		{
			bool flipMe = false;
			{
				ScopeGuard obj([&flipMe] {
							flipMe = true;
						});

				QVERIFY(obj.mFunction);
				QVERIFY(obj.isEnabled());
				obj.setEnabled(false);
				QVERIFY(!obj.isEnabled());
				QVERIFY(!flipMe);
			}
			QVERIFY(!flipMe);
		}


		void enabledAfterCtor()
		{
			bool flipMe = false;
			{
				ScopeGuard obj([&flipMe] {
							flipMe = true;
						}, false);
				QVERIFY(!obj.isEnabled());
				QVERIFY(obj.mFunction);

				obj.setEnabled(true);
				QVERIFY(obj.isEnabled());
				QVERIFY(!flipMe);
			}
			QVERIFY(flipMe);
		}


		void emptyFunc()
		{
			QSignalSpy spy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const std::function<void()> func;

			{
				ScopeGuard obj(func);
				QVERIFY(obj.isEnabled());
				QVERIFY(!obj.mFunction);

				obj.setEnabled(true);
				QVERIFY(obj.isEnabled());
			}

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();

#ifdef Q_OS_WIN
			const QLatin1String lineBreak("\r\n");
#else
			const QLatin1Char lineBreak('\n');
#endif

			QVERIFY(param.at(0).toString().endsWith(QStringLiteral("Cannot call an empty function") + lineBreak));
		}


};

QTEST_GUILESS_MAIN(test_ScopeGuard)
#include "test_ScopeGuard.moc"
