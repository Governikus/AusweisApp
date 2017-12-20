/*!
 * \brief Model implementation for the PIN action.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowModel.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>


namespace governikus
{

class ChangePinContext;

class ChangePinModel
	: public WorkflowModel
{
	Q_OBJECT

	QSharedPointer<ChangePinContext> mContext;

	public:
		ChangePinModel(QObject* pParent = nullptr);
		virtual ~ChangePinModel() override;

		void resetContext(const QSharedPointer<ChangePinContext>& pContext = QSharedPointer<ChangePinContext>());

		virtual QString getResultString() const override;

	Q_SIGNALS:
		void fireNewContextSet();
};


} /* namespace governikus */
