/*!
 * \brief Model implementation for the PIN action.
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "WorkflowModel.h"
#include "WorkflowRequest.h"
#include "context/ChangePinContext.h"

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
		~ChangePinModel() override = default;

	public:
		void resetChangePinContext(const QSharedPointer<ChangePinContext>& pContext = QSharedPointer<ChangePinContext>());

		Q_INVOKABLE void startWorkflow(bool pRequestTransportPin);
		[[nodiscard]] QString getResultString() const override;
		[[nodiscard]] QVector<ReaderManagerPlugInType> getSupportedReaderPlugInTypes() const override;

	private Q_SLOTS:
		void onPaceResultUpdated();

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireNewContextSet();
		void fireOnPinUnlocked();
};


} // namespace governikus
