/*!
 * CompositeStateSelectCard.h
 *
 * \brief Composite state for selecting a card.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
		CompositeStateProcessCvcsAndSetRights(const QSharedPointer<WorkflowContext>& pContext);
		virtual ~CompositeStateProcessCvcsAndSetRights();

	Q_SIGNALS:
		void fireSuccess();
		void fireCancel();
		void fireError();
};

} /* namespace governikus */
