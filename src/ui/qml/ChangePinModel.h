/*!
 * \brief Model implementation for the PIN action.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/ChangePinContext.h"
#include "WorkflowModel.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QString>

class test_ChangePinModel;

namespace governikus
{

class ChangePinModel
	: public WorkflowModel
{
	Q_OBJECT

	private:
		friend class ::test_ChangePinModel;
		QSharedPointer<ChangePinContext> mContext;

	protected:
		ChangePinModel() = default;
		~ChangePinModel() override = default;

	public:
		void resetContext(const QSharedPointer<ChangePinContext>& pContext = QSharedPointer<ChangePinContext>());

		virtual QString getResultString() const override;

		static ChangePinModel& getInstance();

	Q_SIGNALS:
		void fireNewContextSet();
};


} // namespace governikus
