/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SingletonCreator.h"
#include "context/AccessRightManager.h"
#include "context/WorkflowContext.h"

#include <QAbstractListModel>
#include <QList>
#include <QSet>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>


class test_ChatModel;
class test_UiPluginQml;


namespace governikus
{

struct cvcertificate_st;

class ChatModel
	: public QAbstractListModel
	, public SingletonCreator<ChatModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_ChatModel;
	friend class ::test_UiPluginQml;

	Q_PROPERTY(QSortFilterProxyModel * optional READ getFilterOptionalModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel * required READ getFilterRequiredModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel * write READ getFilterWriteModel CONSTANT)

	private:
		QSharedPointer<WorkflowContext> mContext;
		QList<AccessRight> mAllRights;
		QSet<AccessRight> mOptionalRights;
		QSet<AccessRight> mSelectedRights;
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

		ChatModel();
		~ChatModel() override = default;

		void initFilterModel(QSortFilterProxyModel& pModel, QAbstractItemModel* pSourceModel, int pFilterRole, const QString& pFilter) const;
		void setOrderedAllRights(const QSet<AccessRight>& pAllRights);

	private Q_SLOTS:
		void onAuthenticationDataChanged(QSharedPointer<AccessRightManager> pAccessRightManager);

	public:
		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] bool setData(const QModelIndex& pIndex, const QVariant& pValue, int pRole) override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		Q_INVOKABLE void transferAccessRights() const;
		[[nodiscard]] Q_INVOKABLE QSortFilterProxyModel* getFilterOptionalModel();
		[[nodiscard]] Q_INVOKABLE QSortFilterProxyModel* getFilterRequiredModel();
		[[nodiscard]] Q_INVOKABLE QSortFilterProxyModel* getFilterWriteModel();
};


} // namespace governikus
