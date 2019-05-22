/*!
 * \brief Builder for states.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"

#include <QSharedPointer>
#include <QString>

namespace governikus
{

class StateBuilder
{
	StateBuilder() = delete;
	Q_DISABLE_COPY(StateBuilder)

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
