/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Builder for states.
 */

#pragma once

#include "AbstractState.h"

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

	public:
		template<typename T, typename C>
		static T* createState(const QSharedPointer<C>& pContext)
		{
			auto state = new T(pContext);
			state->setStateName(AbstractState::getClassName(state->metaObject()->className()));
			return state;
		}


};

} // namespace governikus
