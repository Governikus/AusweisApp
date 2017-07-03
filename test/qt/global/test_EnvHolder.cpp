/*!
 * \brief Unit tests for \ref EnvHolder
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "EnvHolder.h"

#include <QtTest/QtTest>

using namespace governikus;

namespace
{
class AbstractTestInstance
{
	Q_GADGET

	public:
		virtual ~AbstractTestInstance()
		{
		}


		virtual QString dummy() = 0;
};

class AbstractTestInstanceImpl
	: public AbstractTestInstance
{
	Q_GADGET

	public:
		virtual QString dummy()
		{
			return QStringLiteral("impl");
		}


};

class MockedAbstractTestInstance
	: public AbstractTestInstance
{
	Q_GADGET

	public:
		virtual QString dummy()
		{
			return QStringLiteral("mocked");
		}


};

class TestInstance
{
	Q_GADGET

	protected:
		TestInstance() = default;
		virtual ~TestInstance()
		{
		}


	public:
		static TestInstance& getInstance()
		{
			static TestInstance instance;
			return instance;
		}


		virtual QString something()
		{
			return QStringLiteral("orig");
		}


};

class TestMockedInstance
	: public TestInstance
{
	public:
		virtual QString something() override
		{
			return QStringLiteral("mocked");
		}


};

class TestSharedInstance
	: public QObject
{
	Q_OBJECT

	public:
		virtual QString something()
		{
			return QStringLiteral("orig");
		}


};

class TestMockedSharedInstance
	: public TestSharedInstance
{
	Q_OBJECT

	public:
		virtual QString something() override
		{
			return QStringLiteral("mocked");
		}


};

class TestAbstractUnmanagedInstance
{
	Q_GADGET

	public:
		virtual ~TestAbstractUnmanagedInstance() = default;
		virtual QString something() = 0;
};

class TestUnmanagedInstance
	: public TestAbstractUnmanagedInstance
{
	Q_GADGET

	public:
		virtual QString something() override
		{
			return QStringLiteral("TestUnmanagedInstance");
		}


};


}

namespace governikus
{

template<> bool singleton<TestAbstractUnmanagedInstance*>(TestAbstractUnmanagedInstance*& pPtr)
{
	static TestUnmanagedInstance instance;
	pPtr = &instance;
	return false;
}


template<> bool singleton<AbstractTestInstance*>(AbstractTestInstance*& pPtr)
{
	pPtr = new AbstractTestInstanceImpl;
	return true;
}


template<> AbstractTestInstance* createNewObject<AbstractTestInstance>()
{
	return new AbstractTestInstanceImpl;
}


template<> AbstractTestInstance* createNewObject<AbstractTestInstance, QString>(QString&& pStr)
{
	class tmpCtor
		: public AbstractTestInstance
	{
		private:
			const QString mDummy;

		public:
			tmpCtor(const QString& pDummy) : mDummy(pDummy)
			{
			}


			virtual QString dummy() override
			{
				return mDummy;
			}


	};

	return new tmpCtor(pStr);
}


}


class test_EnvHolder
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void cleanup()
		{
			EnvHolder::clear();
		}


		void mockGet()
		{
			auto orig = EnvHolder::get<TestInstance>();
			QVERIFY(orig);
			QCOMPARE(orig->something(), QLatin1String("orig"));

			TestMockedInstance mock;
			EnvHolder::set(TestInstance::staticMetaObject, &mock);
			auto mocked = EnvHolder::get<TestInstance>();
			QVERIFY(mocked);
			QCOMPARE(mocked->something(), QLatin1String("mocked"));

			EnvHolder::set(TestInstance::staticMetaObject);
			auto orig_again = EnvHolder::get<TestInstance>();
			QVERIFY(orig_again);
			QCOMPARE(orig_again->something(), QLatin1String("orig"));
		}


		void mockAbstract()
		{
			auto impl = EnvHolder::get<AbstractTestInstance>();
			QVERIFY(impl);
			QCOMPARE(impl->dummy(), QLatin1String("impl"));

			auto impl2 = EnvHolder::get<AbstractTestInstance>();
			QVERIFY(impl == impl2);

			MockedAbstractTestInstance m;
			EnvHolder::set(AbstractTestInstance::staticMetaObject, &m);
			auto mocked = EnvHolder::get<AbstractTestInstance>();
			QVERIFY(mocked);
			QVERIFY(mocked != impl);
			QCOMPARE(mocked->dummy(), QLatin1String("mocked"));
		}


		void mockShared()
		{
			auto orig = EnvHolder::shared<TestSharedInstance>();
			QVERIFY(orig);
			QCOMPARE(orig->something(), QLatin1String("orig"));

			auto orig2 = EnvHolder::shared<TestSharedInstance>();
			QVERIFY(orig == orig2);

			auto mock = QSharedPointer<TestMockedSharedInstance>::create();
			EnvHolder::setShared(TestSharedInstance::staticMetaObject, mock);
			auto mocked = EnvHolder::shared<TestSharedInstance>();
			QVERIFY(mocked);
			QCOMPARE(mocked->something(), QLatin1String("mocked"));
			QVERIFY(mock == mocked);
			QVERIFY(orig != mocked);

			EnvHolder::setShared(TestSharedInstance::staticMetaObject);
			auto orig3 = EnvHolder::shared<TestSharedInstance>();
			QVERIFY(orig != orig3);
			QCOMPARE(orig3->something(), QLatin1String("orig"));
		}


		void getUnmanagedSingleton()
		{
			auto first = EnvHolder::get<TestAbstractUnmanagedInstance>();
			QVERIFY(first);
			QCOMPARE(first->something(), QLatin1String("TestUnmanagedInstance"));

			auto second = EnvHolder::get<TestAbstractUnmanagedInstance>();
			QVERIFY(second);
			QVERIFY(first == second);
		}


		void mockCreateNewInstance()
		{
			auto implOrig = EnvHolder::create<AbstractTestInstance>();
			QVERIFY(implOrig);
			QCOMPARE(implOrig->dummy(), QLatin1String("impl"));
			delete implOrig;

			EnvHolder::setCreator(AbstractTestInstance::staticMetaObject, [] {
						class tmp : public AbstractTestInstanceImpl
						{
							virtual QString dummy()
							{
								return QStringLiteral("lambda");
							}
						};
						return new tmp;
					});

			auto impl = EnvHolder::create<AbstractTestInstance>();
			QVERIFY(impl);
			QCOMPARE(impl->dummy(), QLatin1String("lambda"));

			auto impl2 = EnvHolder::create<AbstractTestInstance>();
			QVERIFY(impl2);
			QCOMPARE(impl2->dummy(), QLatin1String("lambda"));
			QVERIFY(impl != impl2);

			delete impl;
			delete impl2;
		}


		void mockArgs()
		{
			auto impl = EnvHolder::create<AbstractTestInstance>(QString("flupp"));
			QVERIFY(impl);
			QCOMPARE(impl->dummy(), QLatin1String("flupp"));
			delete impl;

			EnvHolder::setCreator(AbstractTestInstance::staticMetaObject, [] {
						return new AbstractTestInstanceImpl;
					});

			auto orig = EnvHolder::create<AbstractTestInstance>(QString("flupp"));
			QVERIFY(orig);
			QCOMPARE(orig->dummy(), QLatin1String("impl"));
			delete orig;
		}


};

QTEST_GUILESS_MAIN(test_EnvHolder)
#include "test_EnvHolder.moc"
