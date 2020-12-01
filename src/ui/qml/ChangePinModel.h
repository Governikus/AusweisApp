/*!
 * \brief Model implementation for the PIN action.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/ChangePinContext.h"
#include "Env.h"
#include "WorkflowModel.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class ChangePinModel
	: public WorkflowModel
{
	Q_OBJECT
	friend class Env;

	private:
		QSharedPointer<ChangePinContext> mContext;
		ChangePinModel() = default;
		virtual ~ChangePinModel() override = default;

	public:
		void resetChangePinContext(const QSharedPointer<ChangePinContext>& pContext = QSharedPointer<ChangePinContext>());

		virtual QString getResultString() const override;

	private Q_SLOTS:
		void onPaceResultUpdated();

	Q_SIGNALS:
		void fireNewContextSet();
		void fireOnPinUnlocked();
};


} // namespace governikus
