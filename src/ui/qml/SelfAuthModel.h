/*!
 * \brief Model implementation for the self authentication workflow.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SelfAuthenticationData.h"

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

	QSharedPointer<SelfAuthContext> mContext;
	SelfAuthenticationData::OrderedSelfData mSelfData;

	private Q_SLOTS:
		void onSelfAuthenticationDataChanged();

	protected:
		SelfAuthModel(QObject* pParent = nullptr);
		static SelfAuthModel& getInstance();

	public:
		enum DataRoles
		{
			NAME = Qt::UserRole + 1,
			VALUE
		};

		void resetContext(const QSharedPointer<SelfAuthContext>& pContext = QSharedPointer<SelfAuthContext>());

		Q_INVOKABLE void startWorkflow();
		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE bool isBasicReader();
		Q_INVOKABLE void exportData(const QUrl& pFilename) const;

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

		virtual bool event(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireStartWorkflow();

};


} // namespace governikus
