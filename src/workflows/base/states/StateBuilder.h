/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Builder for states.
 */

#pragma once


#include <QSharedPointer>
#include <QString>

namespace governikus
{

class StateBuilder
{
	Q_DISABLE_COPY(StateBuilder)

	private:
		StateBuilder() = delete;
		~StateBuilder() = delete;

		static QString getUnqualifiedClassName(const char* const pName);

	public:
		template<typename T>
		[[nodiscard]] static QString generateStateName()
		{
			return getUnqualifiedClassName(T::staticMetaObject.className());
		}


		template<typename T>
		static bool isState(const QString& pState)
		{
			return pState == generateStateName<T>();
		}


		template<typename T, typename C>
		static T* createState(const QSharedPointer<C>& pContext)
		{
			auto state = new T(pContext);
			state->setObjectName(getUnqualifiedClassName(state->metaObject()->className()));
			return state;
		}


};

} // namespace governikus
