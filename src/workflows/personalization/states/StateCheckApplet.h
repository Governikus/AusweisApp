/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper state that checks the current Smart-eID state
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
		void fireInstallApplet();
		void fireDeleteApplet();
		void fireDeletePersonalization();

};

} // namespace governikus
