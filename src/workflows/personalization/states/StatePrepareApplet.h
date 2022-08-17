/*!
 * \brief Helper state that installs the Smart-eID applet
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QVariant>


class test_StatePrepareApplet;


namespace governikus
{

class StatePrepareApplet
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StatePrepareApplet;

	private:
		explicit StatePrepareApplet(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
		void setProgress(int pProgress, const QString& pMessage, int pInitialValue = 0, int pMaxValue = 100) const;

	private Q_SLOTS:
		void onCommandDone(const QVariant& pResult);

	public Q_SLOTS:
		void onUserCancelled() override;

};

} // namespace governikus
