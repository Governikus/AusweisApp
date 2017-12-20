/*!
 * \brief Composite state for selecting a card.
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSharedPointer>
#include <QState>


namespace governikus
{

class WorkflowContext;

class CompositeStateSelectCard
	: public QState
{
	Q_OBJECT

	const QSharedPointer<WorkflowContext> mContext;

	public:
		CompositeStateSelectCard(const QSharedPointer<WorkflowContext>& pContext);
		virtual ~CompositeStateSelectCard();

	Q_SIGNALS:
		void fireContinue();
		void fireAbort();
};

} /* namespace governikus */
