/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SingletonCreator.h"
#include "WorkflowModel.h"
#include "WorkflowRequest.h"
#include "context/ChangePinContext.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QString>
#include <QtQml/qqmlregistration.h>


class test_UiPluginQml;


namespace governikus
{

class ChangePinModel
	: public WorkflowModel
	, public SingletonCreator<ChangePinModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_UiPluginQml;
	Q_PROPERTY(bool requestTransportPin READ isRequestTransportPin NOTIFY fireWorkflowStarted)
	Q_PROPERTY(bool onlyCheckPin READ isOnlyCheckPin NOTIFY fireWorkflowStarted)

	private:
		QSharedPointer<ChangePinContext> mContext;
		ChangePinModel() = default;
		~ChangePinModel() override = default;

	public:
		void resetChangePinContext(const QSharedPointer<ChangePinContext>& pContext = QSharedPointer<ChangePinContext>());

		Q_INVOKABLE void startWorkflow(bool pRequestTransportPin, bool pActivateUi = true, bool pOnlyCheckPin = false);
		[[nodiscard]] QString getResultString() const override;
		[[nodiscard]] QList<ReaderManagerPluginType> getSupportedReaderPluginTypes() const override;
		[[nodiscard]] GAnimation getStatusCodeAnimation() const override;
		[[nodiscard]] bool isRequestTransportPin() const;
		[[nodiscard]] bool isOnlyCheckPin() const;

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
};


} // namespace governikus
