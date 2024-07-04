/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the self authentication workflow.
 */

#pragma once

#include "Env.h"
#include "SelfAuthenticationData.h"
#include "SingletonCreator.h"
#include "WorkflowRequest.h"

#include <QAbstractListModel>
#include <QEvent>
#include <QSharedPointer>
#include <QString>
#include <QtQml/qqmlregistration.h>


class test_UiPluginQml;


namespace governikus
{

class SelfAuthContext;

class SelfAuthModel
	: public QAbstractListModel
	, public SingletonCreator<SelfAuthModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_UiPluginQml;

	Q_PROPERTY(bool workflowCancelled READ isWorkflowCancelled NOTIFY fireCancelWorkflow FINAL)

	private:
		SelfAuthModel();

		QSharedPointer<SelfAuthContext> mContext;
		SelfAuthenticationData::OrderedSelfData mSelfData;

	private Q_SLOTS:
		void onSelfAuthenticationDataChanged();

	public:
		enum DataRoles
		{
			NAME = Qt::UserRole + 1,
			VALUE
		};

		void resetContext(const QSharedPointer<SelfAuthContext>& pContext = QSharedPointer<SelfAuthContext>());

		Q_INVOKABLE void startWorkflow(bool pActivateUi = true);
		Q_INVOKABLE void cancelWorkflow();
		[[nodiscard]] bool isWorkflowCancelled() const;
		[[nodiscard]] Q_INVOKABLE bool isBasicReader() const;

		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] bool event(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireCancelWorkflow();
};


} // namespace governikus
