/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper state that installs the Smart-eID applet
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QVariant>


class test_StateInstallApplet;


namespace governikus
{

class StateInstallApplet
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateInstallApplet;

	private:
		explicit StateInstallApplet(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
		void handleEidServiceResult(const EidServiceResult& pResult);

	private Q_SLOTS:
		void onCommandDone(const QVariant& pResult);

	public Q_SLOTS:
		void onUserCancelled() override;

};

} // namespace governikus
