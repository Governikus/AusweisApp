/*
 * \brief Runtime environment to create (mockable) objects.
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <functional>
#include <type_traits>

#include <QCoreApplication>
#include <QDebug>
#include <QMap>
#include <QMetaObject>
#include <QMetaType>
#include <QObject>
#include <QObjectCleanupHandler>
#include <QPointer>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QThread>
#include <QWeakPointer>
#include <QWriteLocker>

#ifndef QT_NO_DEBUG
	#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		#include <QMutableVectorIterator>
	#endif

	#include <QVector>
#endif

class test_Env;

namespace governikus
{

template<typename T> T* singleton();
template<typename T, typename ... Args> T createNewObject(Args && ... pArgs);

class Env
{
	Q_DISABLE_COPY(Env)
	friend class ::test_Env;

	public:
		struct ThreadSafe {};

	private:
		using Identifier = const char*;

#ifndef QT_NO_DEBUG
		class FuncWrapperBase
		{
			int mCounter = 0;

			public:
				[[nodiscard]] inline int getCounter() const
				{
					return mCounter;
				}


				inline void reset()
				{
					mCounter = 0;
				}


				inline void increaseCounter()
				{
					++mCounter;
				}


				virtual ~FuncWrapperBase();
		};

		template<typename T, typename ... Args>
		class FuncWrapper final
			: public FuncWrapperBase
		{
			private:
				const std::function<T(Args ...)> mFunc;

			public:
				explicit FuncWrapper(std::function<T(Args ...)> pFunc)
					: mFunc(std::move(pFunc))
				{
				}


				T operator()(Args&& ... pArgs)
				{
					increaseCounter();
					return mFunc(pArgs ...);
				}


		};

		using Wrapper = QSharedPointer<FuncWrapperBase>;
		QVector<Wrapper> mInstancesCreator;
		QMap<Identifier, void*> mInstancesSingleton;
		mutable QReadWriteLock mLock;
#endif

		QPointer<QObjectCleanupHandler> mSingletonHandler;
		QVector<std::function<void* (bool)>> mSingletonCreator;

		QMap<Identifier, QWeakPointer<QObject>> mSharedInstances;
		mutable QReadWriteLock mSharedInstancesLock;

		static Env& getInstance();

		template<typename T>
		T* createSingleton()
		{
			Q_ASSERT(!mSingletonHandler.isNull());
#ifndef QT_NO_DEBUG
			if (!QCoreApplication::startingUp() && !QCoreApplication::applicationName().startsWith(QLatin1String("Test_")))
			{
				Q_ASSERT(QThread::currentThread()->objectName() == QLatin1String("MainThread"));
			}
#endif

			qDebug() << "Create singleton:" << T::staticMetaObject.className();

			T* ptr = nullptr;
			if constexpr (std::is_abstract_v<T> && std::is_destructible_v<T>)
			{
				ptr = createNewObject<T*>();
			}
			else
			{
				ptr = new T();
			}

			QObject::connect(ptr, &QObject::destroyed, ptr, [] {
					qDebug() << "Destroy singleton:" << T::staticMetaObject.className();
				});
			mSingletonHandler->add(ptr);
			mSingletonCreator << std::bind(&Env::getOrCreateSingleton<T>, this, std::placeholders::_1);

			return ptr;
		}


		template<typename T>
		T* getOrCreateSingleton(bool pCreate = false)
		{
			static QPointer<T> instance = createSingleton<T>();

			if (Q_UNLIKELY(pCreate))
			{
				// It's not thread-safe! So Env::init() should be the only one!
				Q_ASSERT(instance.isNull());
				instance = createSingleton<T>();
			}

			return instance;
		}


		template<typename T>
		inline T* fetchRealSingleton()
		{
			if constexpr (QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value)
			{
				return getOrCreateSingleton<T>();
			}
			else
			{
				if constexpr (std::is_abstract_v<T> && std::is_destructible_v<T>)
				{
					static_assert(std::has_virtual_destructor_v<T>, "Destructor must be virtual");
					return singleton<T>();
				}
				else
				{
					return &T::getInstance();
				}
			}
		}


		template<typename T>
		inline std::enable_if_t<QtPrivate::IsGadgetHelper<T>::IsRealGadget, T*> checkObjectInfo(Identifier pId, T* pObject) const
		{
			Q_UNUSED(pId)
			return pObject;
		}


		template<typename T>
		inline std::enable_if_t<QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value, T*> checkObjectInfo(Identifier pId, T* pObject) const
		{
			if (!std::is_base_of<ThreadSafe, T>() && pObject->thread() != QThread::currentThread())
			{
				qWarning() << pId << "was created in" << pObject->thread()->objectName() << "but is requested by" << QThread::currentThread()->objectName();
#ifndef QT_NO_DEBUG
				Q_ASSERT(QCoreApplication::applicationName().startsWith(QLatin1String("Test_global_Env")));
#endif
			}

			return pObject;
		}


		template<typename T>
		inline T* fetchSingleton()
		{
			static_assert(QtPrivate::IsGadgetHelper<T>::IsRealGadget || QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value,
					"Singletons needs to be a Q_GADGET or an QObject/Q_OBJECT");

			const Identifier id = T::staticMetaObject.className();
			void* obj = nullptr;
#ifndef QT_NO_DEBUG
			const QReadLocker locker(&mLock);
			obj = mInstancesSingleton.value(id);
			if (!obj)
#endif
			obj = fetchRealSingleton<T>();
			Q_ASSERT(obj);
			return checkObjectInfo(id, static_cast<T*>(obj));
		}


		template<typename T, typename ... Args>
		inline T newObject(Args&& ... pArgs) const
		{
			if constexpr (std::is_constructible_v<std::remove_pointer_t<T>, Args ...>)
			{
				if constexpr (std::is_pointer_v<T>)
				{
					using t = std::remove_pointer_t<T>;
					return new t(std::forward<Args>(pArgs) ...);
				}
				else
				{
					return T(std::forward<Args>(pArgs) ...);
				}
			}
			else
			{
				static_assert(std::is_pointer_v<T>, "It is impossible to return implementation of interface by value. Use pointer or add constructor!");
				auto obj = createNewObject<T>(std::forward<Args>(pArgs) ...);
				Q_ASSERT(obj);
				return obj;
			}
		}


		template<typename T, typename ... Args>
		T createObject(Args&& ... pArgs) const
		{
#ifndef QT_NO_DEBUG
			{
				QReadLocker locker(&mLock);

				// copy QSharedPointer "mock" to increase ref-counter. Otherwise
				// unlock would allow to delete the wrapper.
				for (auto mock : std::as_const(mInstancesCreator)) // clazy:exclude=range-loop,range-loop-reference
				{
					auto creator = mock.dynamicCast<FuncWrapper<T, Args ...>>();
					if (creator)
					{
						locker.unlock();
						return (*creator)(std::forward<Args>(pArgs) ...);
					}
				}
			}
#endif

			return newObject<T>(std::forward<Args>(pArgs) ...);
		}


		void initialize()
		{
			Q_ASSERT(mSingletonHandler.isNull());
			mSingletonHandler = new QObjectCleanupHandler();
			QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, mSingletonHandler.data(), &QObject::deleteLater);

			const auto copy = mSingletonCreator;
			mSingletonCreator.clear();
			for (const auto& func : copy)
			{
				func(true);
			}
		}

	protected:
		Env();
		~Env() = default;

	public:
		static void init()
		{
			getInstance().initialize();
		}


		template<typename T>
		static T* getSingleton()
		{
			return getInstance().fetchSingleton<T>();
		}


		template<typename T, typename ... Args>
		static T create(Args&& ... pArgs)
		{
			return getInstance().createObject<T>(std::forward<Args>(pArgs) ...);
		}


		template<typename T>
		static QSharedPointer<T> getShared()
		{
			static_assert(QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value, "Shared class needs to be an QObject/Q_OBJECT");

			const Identifier className = T::staticMetaObject.className();

			auto& holder = getInstance();
			holder.mSharedInstancesLock.lockForRead();
			QSharedPointer<T> shared = qSharedPointerCast<T>(holder.mSharedInstances.value(className));
			holder.mSharedInstancesLock.unlock();

			if (!shared)
			{
				const QWriteLocker locker(&holder.mSharedInstancesLock);
				shared = qSharedPointerCast<T>(holder.mSharedInstances.value(className));
				if (!shared)
				{
					qDebug() << "Spawn shared instance:" << className;
					shared = QSharedPointer<T>::create();
					holder.mSharedInstances.insert(className, shared.toWeakRef());
				}
			}

			return shared;
		}


#ifndef QT_NO_DEBUG
		static void resetCounter();
		static void clear();
		static void set(const QMetaObject& pMetaObject, void* pObject = nullptr);

		template<typename T, typename ... Args>
		static int getCounter()
		{
			auto& holder = getInstance();
			const QReadLocker locker(&holder.mLock);

			for (const auto& mock : std::as_const(holder.mInstancesCreator))
			{
				if (mock.dynamicCast<FuncWrapper<T, Args ...>>())
				{
					return mock->getCounter();
				}
			}

			return -1; // There is no mock...  use setCreator!
		}


		template<typename T, typename ... Args>
		static void setCreator(std::function<T(Args ...)> pFunc)
		{
			Q_ASSERT(pFunc);

			const auto& value = QSharedPointer<FuncWrapper<T, Args ...>>::create(std::move(pFunc));

			auto& holder = getInstance();
			const QWriteLocker locker(&holder.mLock);

			QMutableVectorIterator<Wrapper> iter(holder.mInstancesCreator);
			while (iter.hasNext())
			{
				iter.next();
				if (iter.value().dynamicCast<FuncWrapper<T, Args ...>>())
				{
					iter.setValue(value);
					return;
				}
			}

			holder.mInstancesCreator << value;
		}


		static void setShared(const QMetaObject& pMetaObject, const QSharedPointer<QObject>& pObject);
#endif

};

} // namespace governikus
