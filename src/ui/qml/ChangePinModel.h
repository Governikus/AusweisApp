/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the PIN action.
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


class test_UIPlugInQml;


namespace governikus
{

class ChangePinModel
	: public WorkflowModel
{
	Q_OBJECT
	friend class Env;
	friend class ::test_UIPlugInQml;
	Q_PROPERTY(bool requestTransportPin READ isRequestTransportPin NOTIFY fireWorkflowStarted)

	private:
		QSharedPointer<ChangePinContext> mContext;
		ChangePinModel() = default;
		~ChangePinModel() override = default;

	public:
		void resetChangePinContext(const QSharedPointer<ChangePinContext>& pContext = QSharedPointer<ChangePinContext>());

		Q_INVOKABLE void startWorkflow(bool pRequestTransportPin, bool pActivateUi = true);
		[[nodiscard]] QString getResultString() const override;
		[[nodiscard]] QList<ReaderManagerPlugInType> getSupportedReaderPlugInTypes() const override;
		[[nodiscard]] bool isRequestTransportPin() const;

	private Q_SLOTS:
		void onPaceResultUpdated();

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireOnPinUnlocked();
		void fireOnPasswordUsed();
};


} // namespace governikus
