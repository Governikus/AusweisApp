/*!
 * \brief Model implementation for the chat.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QSet>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "context/AuthContext.h"
#include "Env.h"

class test_ChatModel;

namespace governikus
{

struct cvcertificate_st;

class ChatModel
	: public QAbstractListModel
{
	Q_OBJECT
	friend class Env;
	friend class ::test_ChatModel;

	Q_PROPERTY(QSortFilterProxyModel * optional READ getFilterOptionalModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel * required READ getFilterRequiredModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel * write READ getFilterWriteModel CONSTANT)

	QSharedPointer<AuthContext> mAuthContext;
	QList<AccessRight> mAllRights;
	QSet<AccessRight> mOptionalRights, mSelectedRights;
	QSortFilterProxyModel mFilterOptionalModel;
	QSortFilterProxyModel mFilterRequiredModel;
	QSortFilterProxyModel mFilterReadModel;
	QSortFilterProxyModel mFilterWriteModel;

	enum ChatRoles
	{
		NAME_ROLE = Qt::UserRole + 1,
		OPTIONAL_ROLE,
		SELECTED_ROLE,
		WRITE_RIGHT,
	};

	private:
		ChatModel();
		virtual ~ChatModel() override = default;

		void initFilterModel(QSortFilterProxyModel& pModel, QAbstractItemModel* pSourceModel, int pFilterRole, const QString& pFilter);
		void setOrderedAllRights(const QSet<AccessRight>& pAllRights);

	private Q_SLOTS:
		void onAuthenticationDataChanged(QSharedPointer<AccessRightManager> pAccessRightManager);

	public:
		void resetContext(const QSharedPointer<AuthContext>& pContext = QSharedPointer<AuthContext>());

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		bool setData(const QModelIndex& pIndex, const QVariant& pValue, int pRole) override;
		QHash<int, QByteArray> roleNames() const override;

		Q_INVOKABLE void transferAccessRights();
		Q_INVOKABLE QSortFilterProxyModel* getFilterOptionalModel();
		Q_INVOKABLE QSortFilterProxyModel* getFilterRequiredModel();
		Q_INVOKABLE QSortFilterProxyModel* getFilterWriteModel();
};


} // namespace governikus
