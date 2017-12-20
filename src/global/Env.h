/*
 * \brief Runtime environment to create (mockable) objects.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <functional>
#include <memory>
#include <QMap>
#include <QMetaObject>
#include <QMetaType>
#include <QObject>
#include <QSharedPointer>
#include <QWeakPointer>

#ifndef QT_NO_DEBUG
#include <QMutableVectorIterator>
#include <QVector>
#endif

class test_Env;

namespace governikus
{

template<typename T> T* singleton(bool& pTakeOwnership);
template<typename T, typename ... Args> T createNewObject(Args&& ... pArgs);

class Env
{
	private:
		friend class ::test_Env;
		Q_DISABLE_COPY(Env)

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
				FuncWrapper(const std::function<T(Args ...)>& pFunc)
					: mFunc(pFunc)
				{
				}


				virtual ~FuncWrapper()
				{
				}


				T operator()(Args&& ... pArgs)
				{
					++mCounter;
					return mFunc(std::forward<Args>(pArgs) ...);
				}


		};

		using Wrapper = std::shared_ptr<FuncWrapperBase>;
		QVector<Wrapper> mInstancesCreator;
#endif


		enum class Type
		{
			UNDEFINED,
			OWNERSHIP,
			UNMANAGED
		};

		using Identifier = const char*;
		QMap<Identifier, Type> mTypeInfo;
		QMap<Identifier, void*> mInstancesUnmanaged;
		QMap<Identifier, std::shared_ptr<void> > mInstancesOwnership;
		QMap<Identifier, QWeakPointer<QObject> > mSharedInstances;

		static Env& getInstance();

		void storeSingleton(Identifier pId, void* pObject);
		void storeSingleton(Identifier pId, std::shared_ptr<void> pObject);
		void removeStoredSingleton(Identifier pId);
		void* fetchStoredSingleton(Identifier pId) const;

		template<typename T>
		typename std::enable_if<std::is_abstract<T>::value && std::is_destructible<T>::value, T*>::type storeSingleton(Identifier pId)
		{
			static_assert(std::has_virtual_destructor<T>::value, "Destructor must be virtual");

			bool ownership = true;
			T* obj = singleton<T>(ownership);
			Q_ASSERT(obj);
			if (ownership)
			{
				storeSingleton(pId, std::shared_ptr<void>(obj));
			}
			else
			{
				storeSingleton(pId, obj);
			}
			return obj;
		}


		template<typename T>
		typename std::enable_if<!std::is_destructible<T>::value, T*>::type storeSingleton(Identifier pId)
		{
			T* obj = &T::getInstance();
			storeSingleton(pId, obj);
			return obj;
		}


		template<typename T>
		typename std::enable_if<std::is_default_constructible<T>::value, T*>::type storeSingleton(Identifier pId)
		{
			auto obj = std::make_shared<T>();
			storeSingleton(pId, obj);
			return obj.get();
		}


		template<typename T>
		inline T* fetchSingleton()
		{
			static_assert(QtPrivate::IsGadgetHelper<T>::Value || QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value,
					"Singletons needs to be a Q_GADGET or an QObject/Q_OBJECT");
			Identifier id = T::staticMetaObject.className();
			void* obj = fetchStoredSingleton(id);

			if (!obj)
			{
				obj = storeSingleton<T>(id);
			}

			return static_cast<T*>(obj);
		}


		template<typename T, typename ... Args>
		inline typename std::enable_if<!std::is_constructible<typename std::remove_pointer<T>::type, Args ...>::value, T>::type newObject(Args&& ... pArgs) const
		{
			static_assert(std::is_pointer<T>::value, "It is impossible to return implementation of interface by value. Use pointer or add constructor!");
			auto obj = createNewObject<T>(std::forward<Args>(pArgs) ...);
			Q_ASSERT(obj);
			return obj;
		}


		template<typename T, typename ... Args>
		inline typename std::enable_if<std::is_pointer<T>::value, T>::type internalNewObject(Args&& ... pArgs) const
		{
			using t = typename std::remove_pointer<T>::type;
			return new t(std::forward<Args>(pArgs) ...);
		}


		template<typename T, typename ... Args>
		inline typename std::enable_if<!std::is_pointer<T>::value, T>::type internalNewObject(Args&& ... pArgs) const
		{
			return T(std::forward<Args>(pArgs) ...);
		}


		template<typename T, typename ... Args>
		inline typename std::enable_if<std::is_constructible<typename std::remove_pointer<T>::type, Args ...>::value, T>::type newObject(Args&& ... pArgs) const
		{
			return internalNewObject<T>(std::forward<Args>(pArgs) ...);
		}


		template<typename T, typename ... Args>
		T createObject(Args&& ... pArgs) const
		{
#ifndef QT_NO_DEBUG
			for (auto& mock : qAsConst(mInstancesCreator))
			{
				auto creator = dynamic_cast<FuncWrapper<T, Args ...>*>(mock.get());
				if (creator)
				{
					return (*creator)(std::forward<Args>(pArgs) ...);
				}
			}
#endif

			return newObject<T>(std::forward<Args>(pArgs) ...);
		}


	protected:
		Env();
		~Env();

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
			static_assert(QtPrivate::IsGadgetHelper<T>::Value || QtPrivate::IsPointerToTypeDerivedFromQObject<T*>::Value,
					"Shared class needs to be a Q_GADGET or an QObject/Q_OBJECT");

			auto& holder = getInstance().mSharedInstances;
			const auto* className = T::staticMetaObject.className();

			QSharedPointer<T> shared = qSharedPointerCast<T>(holder.value(className));
			if (!shared)
			{
				shared = QSharedPointer<T>::create();
				holder.insert(className, shared.toWeakRef());
			}

			return shared;
		}


#ifndef QT_NO_DEBUG
		static void resetCounter();
		static void clear();
		static void set(const QMetaObject& pMetaObject, void* pObject = nullptr);
		static void set(const QMetaObject& pMetaObject, std::shared_ptr<void> pObject);

		template<typename T, typename U>
		static U* getSingleton()
		{
			return dynamic_cast<U*>(getSingleton<T>());
		}


		template<typename T, typename ... Args>
		static int getCounter()
		{
			for (const auto& mock : qAsConst(getInstance().mInstancesCreator))
			{
				if (dynamic_cast<const FuncWrapper<T, Args ...>*>(mock.get()))
				{
					return mock->getCounter();
				}
			}

			return -1; // There is no mock...  use setCreator!
		}


		template<typename T, typename ... Args>
		static void setCreator(const std::function<T(Args ...)>& pFunc)
		{
			Q_ASSERT(pFunc);

			auto& holder = getInstance().mInstancesCreator;
			const auto& value = Wrapper(new FuncWrapper<T, Args ...>(pFunc));

			QMutableVectorIterator<Wrapper> iter(holder);
			while (iter.hasNext())
			{
				iter.next();
				if (dynamic_cast<const FuncWrapper<T, Args ...>*>(iter.value().get()))
				{
					iter.setValue(value);
					return;
				}
			}

			holder << value;
		}


		static void setShared(const QMetaObject& pMetaObject, QSharedPointer<QObject> pObject = QSharedPointer<QObject>());
#endif

};

} /* namespace governikus */
