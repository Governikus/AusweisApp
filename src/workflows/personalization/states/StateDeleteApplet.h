/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper state that deletes the Smart-eID applet
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QVariant>


class test_StateDeleteApplet;


namespace governikus
{

class StateDeleteApplet
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateDeleteApplet;

	private:
		explicit StateDeleteApplet(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
		void handleEidServiceResult(const EidServiceResult& pResult);

	private Q_SLOTS:
		void onCommandDone(const QVariant& pResult);

	public Q_SLOTS:
		void onUserCancelled() override;

};

} // namespace governikus
