/*!
 * \brief Composite state for selecting a card.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
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
		virtual ~CompositeStateProcessCvcsAndSetRights();

	Q_SIGNALS:
		void fireContinue();
		void fireAbort();
};

} // namespace governikus
