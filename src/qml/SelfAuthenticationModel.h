/*!
 * \brief Model implementation for the self authentication workflow.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QAbstractListModel>
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QVector>

namespace governikus
{

class SelfAuthenticationContext;

class SelfAuthenticationModel
	: public QAbstractListModel
{
	Q_OBJECT

	QSharedPointer<SelfAuthenticationContext> mContext;
	QVector<QPair<QString, QString> > mData;

	enum DataRoles
	{
		NAME = Qt::UserRole + 1,
		VALUE
	};

	private Q_SLOTS:
		void onSelfAuthenticationDataChanged();

	public:
		SelfAuthenticationModel(QObject* pParent = nullptr);
		void resetContext(const QSharedPointer<SelfAuthenticationContext>& pContext = QSharedPointer<SelfAuthenticationContext>());

		Q_INVOKABLE void startWorkflow();
		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE bool isBasicReader();

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

	Q_SIGNALS:
		void fireStartWorkflow();

};


} /* namespace governikus */
