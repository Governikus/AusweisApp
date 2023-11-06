/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper state that updates the Smart-eID support info
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QVariant>


class test_StateUpdateSupportInfo;


namespace governikus
{

class StateUpdateSupportInfo
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateUpdateSupportInfo;

	private:
		explicit StateUpdateSupportInfo(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
		void handleEidServiceResult(const EidServiceResult& pResult);
		void handleEidSupportStatus(const EidSupportStatus& pStatus);

	private Q_SLOTS:
		void onCommandDone(const QVariant& pResult);

	public Q_SLOTS:
		void onUserCancelled() override;

	Q_SIGNALS:
		void fireUpdateAvailable();

};

} // namespace governikus
