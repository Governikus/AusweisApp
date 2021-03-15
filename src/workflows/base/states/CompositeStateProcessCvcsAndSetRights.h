/*!
 * \brief Composite state for selecting a card.
 *
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSharedPointer>
#include <QState>


namespace governikus
{

class WorkflowContext;

class CompositeStateProcessCvcsAndSetRights
	: public QState
{
	Q_OBJECT

	const QSharedPointer<WorkflowContext> mContext;

	public:
		explicit CompositeStateProcessCvcsAndSetRights(const QSharedPointer<WorkflowContext>& pContext);
		~CompositeStateProcessCvcsAndSetRights() override;

	Q_SIGNALS:
		void fireContinue();
		void fireAbort();
};

} // namespace governikus
