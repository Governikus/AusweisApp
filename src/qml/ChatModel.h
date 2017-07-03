/*!
 * \brief Model implementation for the chat.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QSet>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "context/AuthContext.h"

namespace governikus
{

struct cvcertificate_st;

class ChatModel
	: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(QSortFilterProxyModel * optional READ getFilterOptionalModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel * required READ getFilterRequiredModel CONSTANT)

	QSharedPointer<AuthContext> mAuthContext;
	QList<AccessRight> mAllRights;
	QSet<AccessRight> mOptionalRights, mSelectedRights;
	QSortFilterProxyModel mFilterOptionalModel;
	QSortFilterProxyModel mFilterRequiredModel;
	QString mRequiredAge;

	enum ChatRoles
	{
		NAME = Qt::UserRole + 1,
		OPTIONAL,
		SELECTED
	};

	private:
		void initFilterModel(QSortFilterProxyModel& pModel, const QString& pFilter);
		void setOrderedAllRights(const QSet<AccessRight>& pAllRights);

	private Q_SLOTS:
		void onOptionalChatChanged();
		void onRequiredChatChanged();
		void onRequiredAgeChanged();

	public:
		ChatModel(QObject* pParent = nullptr);

		void resetContext(QSharedPointer<AuthContext> pContext = QSharedPointer<AuthContext>());

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		bool setData(const QModelIndex& pIndex, const QVariant& pValue, int pRole) override;
		QHash<int, QByteArray> roleNames() const override;

		Q_INVOKABLE void transferAccessRights();
		Q_INVOKABLE QSortFilterProxyModel* getFilterOptionalModel();
		Q_INVOKABLE QSortFilterProxyModel* getFilterRequiredModel();
};


} /* namespace governikus */
