/*
 * \brief Runtime environment to create (mockable) objects.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
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
#include <QReadLocker>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QThread>
#include <QWeakPointer>
#include <QWriteLocker>

#ifndef QT_NO_DEBUG
#include <QMutableVectorIterator>
#include <QVector>
#endif

class test_Env;

namespace governikus
{

template<typename T> T* singleton();
template<typename T, typename ... Args> T createNewObject(Args&& ... pArgs);

class Env
{
	public:
		struct ThreadSafe {};

	private:
		friend class ::test_Env;
		Q_DISABLE_COPY(Env)
		using Identifier = const char*;

#ifndef QT_NO_DEBUG
		class FuncWrapperBase
		{
			protected:
				int mCounter = 0;

			public:
				inline int getCounter() const
				{
					return mCounter;
				}


				inline void reset()
				{
					mCounter = 0;
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
				FuncWrapper(std::function<T(Args ...)> pFunc)
					: mFunc(std::move(pFunc))
				{
				}


				T operator()(Args&& ... pArgs)
				{
					++mCounter;
					return mFunc(std::forward<Args>(pArgs) ...);
				}


		};

		using Wrapper = QSharedPointer<FuncWrapperBase>;
		QVector<Wrapper> mInstancesCreator;
		QMap<Identifier, void*> mInstancesSingleton;
		mutable QReadWriteLock mLock;
#endif

		QMap<Identifier, QWeakPointer<QObject> > mSharedInstances;
		mutable QReadWriteLock mSharedInstancesLock;

		static Env& getInstance();

		template<typename T>
		inline T* fetchRealSingleton()
		{
			if constexpr (std::is_abstract<T>::value && std::is_destructible<T>::value)
			{
				static_assert(std::has_virtual_destructor<T>::value, "Destructor must be virtual");
				return singleton<T>();
			}
			else
			{
				return &T::getInstance();
			}
		}


		template<typename T>
		#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
		inline typename std::enable_if<QtPrivate::IsGadgetHelper<T>::IsRealGadget, T*>::type checkObjectInfo(Identifier pId, T* pObject) const

		#else
		inline typename std::enable_if<QtPrivate::IsGadgetHelper<T>::Value, T*>::type checkObjectInfo(Identifier pId, T* pObject) const
		#endif
		{
			Q_UNUSED(pId)
			return pObject;
		}


		template<typename T>
		inline typename std::enable_if<QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value, T*>::type checkObjectInfo(Identifier pId, T* pObject) const
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
			#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
			static_assert(QtPrivate::IsGadgetHelper<T>::IsRealGadget || QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value,
					"Singletons needs to be a Q_GADGET or an QObject/Q_OBJECT");
			#else
			static_assert(QtPrivate::IsGadgetHelper<T>::Value || QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value,
					"Singletons needs to be a Q_GADGET or an QObject/Q_OBJECT");
			#endif

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
			if constexpr (std::is_constructible<typename std::remove_pointer<T>::type, Args ...>::value)
			{
				if constexpr (std::is_pointer<T>::value)
				{
					using t = typename std::remove_pointer<T>::type;
					return new t(std::forward<Args>(pArgs) ...);
				}
				else
				{
					return T(std::forward<Args>(pArgs) ...);
				}
			}
			else
			{
				static_assert(std::is_pointer<T>::value, "It is impossible to return implementation of interface by value. Use pointer or add constructor!");
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
				for (auto mock : qAsConst(mInstancesCreator)) // clazy:exclude=range-loop
				{
					auto creator = mock.dynamicCast<FuncWrapper<T, Args ...> >();
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


	protected:
		Env();
		~Env() = default;

	public:
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
			#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
			static_assert(QtPrivate::IsGadgetHelper<T>::IsRealGadget || QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value,
					"Shared class needs to be a Q_GADGET or an QObject/Q_OBJECT");
			#else
			static_assert(QtPrivate::IsGadgetHelper<T>::Value || QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value,
					"Shared class needs to be a Q_GADGET or an QObject/Q_OBJECT");
			#endif

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

			for (const auto& mock : qAsConst(holder.mInstancesCreator))
			{
				if (mock.dynamicCast<FuncWrapper<T, Args ...> >())
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

			const auto& value = QSharedPointer<FuncWrapper<T, Args ...> >::create(std::move(pFunc));

			auto& holder = getInstance();
			const QWriteLocker locker(&holder.mLock);

			QMutableVectorIterator<Wrapper> iter(holder.mInstancesCreator);
			while (iter.hasNext())
			{
				iter.next();
				if (iter.value().dynamicCast<FuncWrapper<T, Args ...> >())
				{
					iter.setValue(value);
					return;
				}
			}

			holder.mInstancesCreator << value;
		}


		static void setShared(const QMetaObject& pMetaObject, QSharedPointer<QObject> pObject = QSharedPointer<QObject>());
#endif

};

} // namespace governikus
