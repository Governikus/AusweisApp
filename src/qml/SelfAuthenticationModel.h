/*!
 * \brief Model implementation for the self authentication workflow.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SelfAuthenticationData.h"

#include <QAbstractListModel>
#include <QEvent>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class SelfAuthContext;

class SelfAuthenticationModel
	: public QAbstractListModel
{
	Q_OBJECT

	QSharedPointer<SelfAuthContext> mContext;
	SelfAuthenticationData::OrderedSelfData mSelfData;

	enum DataRoles
	{
		NAME = Qt::UserRole + 1,
		VALUE
	};

	private Q_SLOTS:
		void onSelfAuthenticationDataChanged();

	public:
		SelfAuthenticationModel(QObject* pParent = nullptr);
		void resetContext(const QSharedPointer<SelfAuthContext>& pContext = QSharedPointer<SelfAuthContext>());

		Q_INVOKABLE void startWorkflow();
		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE bool isBasicReader();

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

		virtual bool event(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireStartWorkflow();

};


} /* namespace governikus */
