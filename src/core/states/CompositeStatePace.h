/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSharedPointer>
#include <QState>


namespace governikus
{

class WorkflowContext;

class CompositeStatePace
	: public QState
{
	Q_OBJECT

	const QSharedPointer<WorkflowContext> mContext;

	public:
		explicit CompositeStatePace(const QSharedPointer<WorkflowContext>& pContext);
		virtual ~CompositeStatePace() = default;

	Q_SIGNALS:
		void firePaceChannelEstablished();
		void firePacePukEstablished();
		void fireAbort();
};

} // namespace governikus
