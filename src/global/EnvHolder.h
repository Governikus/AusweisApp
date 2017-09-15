/*
 * \brief Runtime environment holder for all Singletons.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include <functional>
#include <memory>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QSharedPointer>
#include <QWeakPointer>

namespace governikus
{

template<typename T> bool singleton(T&);
template<typename T, typename ... Args> T* createNewObject(Args&& ... pArgs);

class EnvHolder
{
	private:
		Q_DISABLE_COPY(EnvHolder)

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
#ifndef QT_NO_DEBUG
		QMap<Identifier, std::function<void*()> > mInstancesCreator;
#endif

		static EnvHolder& getInstance();

		template<typename T>
		typename std::enable_if<std::is_abstract<T>::value && std::is_destructible<T>::value, T*>::type storeSingleton(Identifier pId)
		{
			static_assert(std::has_virtual_destructor<T>::value, "Destructor must be virtual");

			T* obj = nullptr;
			bool ownership = singleton<T*>(obj);
			Q_ASSERT(obj);
			mTypeInfo.insert(pId, ownership ? Type::OWNERSHIP : Type::UNMANAGED);
			if (ownership)
			{
				mInstancesOwnership.insert(pId, std::shared_ptr<void>(obj));
			}
			else
			{
				mInstancesUnmanaged.insert(pId, obj);
			}
			return obj;
		}


		template<typename T>
		typename std::enable_if<!std::is_destructible<T>::value, T*>::type storeSingleton(Identifier pId)
		{
			T* obj = &T::getInstance();
			mInstancesUnmanaged.insert(pId, obj);
			mTypeInfo.insert(pId, Type::UNMANAGED);
			return obj;
		}


		template<typename T>
		typename std::enable_if<std::is_default_constructible<T>::value, T*>::type storeSingleton(Identifier pId)
		{
			T* obj = new T;
			mInstancesOwnership.insert(pId, std::shared_ptr<void>(obj));
			mTypeInfo.insert(pId, Type::OWNERSHIP);
			return obj;
		}


		void* getStoredSingleton(Identifier pId) const;

		template<typename T>
		inline T* getSingleton()
		{
			Identifier id = T::staticMetaObject.className();
			void* obj = getStoredSingleton(id);

			if (!obj)
			{
				obj = storeSingleton<T>(id);
			}

			return static_cast<T*>(obj);
		}


		template<typename T, typename ... Args>
		inline typename std::enable_if<!std::is_default_constructible<T>::value, T*>::type newObject(Args&& ... pArgs) const
		{
			auto obj = createNewObject<T>(std::forward<Args>(pArgs) ...);
			Q_ASSERT(obj);
			return obj;
		}


		template<typename T, typename ... Args>
		inline typename std::enable_if<std::is_default_constructible<T>::value, T*>::type newObject(Args&& ... pArgs) const
		{
			return new T(std::forward<Args>(pArgs) ...);
		}


		template<typename T, typename ... Args>
		inline T* createObject(Args&& ... pArgs) const
		{
#ifndef QT_NO_DEBUG
			Identifier id = T::staticMetaObject.className();
			auto creator = mInstancesCreator.value(id);

			if (creator)
			{
				return static_cast<T*>(creator());
			}
#endif
			return newObject<T>(std::forward<Args>(pArgs) ...);
		}


	protected:
		EnvHolder();
		~EnvHolder();

	public:
		template<typename T>
		static T* get()
		{
			return getInstance().getSingleton<T>();
		}


		template<typename T, typename ... Args>
		static T* create(Args&& ... pArgs)
		{
			return getInstance().createObject<T>(std::forward<Args>(pArgs) ...);
		}


		template<typename T>
		static QSharedPointer<T> shared()
		{
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
		static void clear();
		static void set(const QMetaObject& pMetaObject, void* pObject = nullptr, bool pOwnership = false);
		static void setCreator(const QMetaObject& pMetaObject, const std::function<void*()>& pFunc);
		static void setShared(const QMetaObject& pMetaObject, QSharedPointer<QObject> pObject = QSharedPointer<QObject>());
#endif

};

} /* namespace governikus */
