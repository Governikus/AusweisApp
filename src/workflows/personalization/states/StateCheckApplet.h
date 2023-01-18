/*!
 * \brief Helper state that installs the Smart-eID applet
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QVariant>


class test_StateCheckApplet;


namespace governikus
{

class StateCheckApplet
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateCheckApplet;

	private:
		explicit StateCheckApplet(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCommandDone(const QVariant& pResult);

	Q_SIGNALS:
		void fireFurtherStepRequired();

};

} // namespace governikus
