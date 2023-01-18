/*!
 * \brief Model implementation for the self authentication workflow.
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SelfAuthenticationData.h"
#include "WorkflowRequest.h"

#include <QAbstractListModel>
#include <QEvent>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class SelfAuthContext;

class SelfAuthModel
	: public QAbstractListModel
{
	Q_OBJECT
	friend class Env;

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

		Q_INVOKABLE void startWorkflow();
		Q_INVOKABLE void cancelWorkflow();
		[[nodiscard]] Q_INVOKABLE bool isBasicReader() const;
		Q_INVOKABLE void exportData(const QUrl& pFilename) const;

		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] bool event(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
};


} // namespace governikus
