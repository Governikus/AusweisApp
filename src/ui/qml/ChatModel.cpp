/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the chat.
 */

#include "ChatModel.h"

#include "AppSettings.h"
#include "asn1/AccessRoleAndRight.h"
#include "context/AuthContext.h"
#include "context/IfdServiceContext.h"

using namespace governikus;

ChatModel::ChatModel()
	: QAbstractListModel()
	, mContext()
	, mAllRights()
	, mOptionalRights()
	, mSelectedRights()
	, mFilterOptionalModel()
	, mFilterRequiredModel()
	, mFilterReadModel()
	, mFilterWriteModel()
{
	resetContext(QSharedPointer<AuthContext>());

	initFilterModel(mFilterReadModel, this, ChatRoles::WRITE_RIGHT, QStringLiteral("false"));
	initFilterModel(mFilterWriteModel, this, ChatRoles::WRITE_RIGHT, QStringLiteral("true"));
	initFilterModel(mFilterOptionalModel, &mFilterReadModel, ChatRoles::OPTIONAL_ROLE, QStringLiteral("true"));
	initFilterModel(mFilterRequiredModel, &mFilterReadModel, ChatRoles::OPTIONAL_ROLE, QStringLiteral("false"));

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireSettingsChanged, this, [this]()
		{
			beginResetModel();
			endResetModel();
		});
}


void ChatModel::initFilterModel(QSortFilterProxyModel& pModel, QAbstractItemModel* pSourceModel, int pFilterRole, const QString& pFilter) const
{
	pModel.setSourceModel(pSourceModel);
	pModel.setFilterRole(pFilterRole);
	pModel.setFilterRegularExpression(pFilter);
}


void ChatModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;

	beginResetModel();

	mAllRights.clear();
	mOptionalRights.clear();
	mSelectedRights.clear();

	endResetModel();

	if (auto authContext = pContext.objectCast<AuthContext>())
	{
		connect(authContext.data(), &AuthContext::fireAccessRightManagerCreated, this, &ChatModel::onAuthenticationDataChanged);
	}
	else if (auto ifdContext = pContext.objectCast<IfdServiceContext>())
	{
		connect(ifdContext.data(), &IfdServiceContext::fireAccessRightManagerCreated, this, &ChatModel::onAuthenticationDataChanged);
	}
}


void ChatModel::onAuthenticationDataChanged(QSharedPointer<AccessRightManager> pAccessRightManager)
{
	beginResetModel();

	mAllRights.clear();
	mOptionalRights.clear();
	mSelectedRights.clear();

	if (!pAccessRightManager->getRequiredAccessRights().isEmpty())
	{
		setOrderedAllRights(pAccessRightManager->getRequiredAccessRights());
		mSelectedRights += pAccessRightManager->getRequiredAccessRights();
	}

	if (!pAccessRightManager->getOptionalAccessRights().isEmpty())
	{
		mOptionalRights += pAccessRightManager->getOptionalAccessRights();
		setOrderedAllRights(pAccessRightManager->getOptionalAccessRights());
		mSelectedRights += pAccessRightManager->getOptionalAccessRights();
	}

	endResetModel();
}


void ChatModel::setOrderedAllRights(const QSet<AccessRight>& pAllRights)
{
	for (auto right : AccessRoleAndRightsUtil::allDisplayedOrderedRights())
	{
		if (pAllRights.contains(right))
		{
			mAllRights += right;
		}
	}
}


int ChatModel::rowCount(const QModelIndex&) const
{
	return static_cast<int>(mAllRights.size());
}


QVariant ChatModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (!pIndex.isValid() || pIndex.row() >= rowCount())
	{
		return QVariant();
	}

	const auto& right = mAllRights.at(pIndex.row());
	switch (pRole)
	{
		case Qt::DisplayRole:
		case NAME_ROLE:
		{
			QString displayText = AccessRoleAndRightsUtil::toDisplayText(right);
			if (right == AccessRight::AGE_VERIFICATION)
			{
				if (auto authContext = mContext.objectCast<AuthContext>())
				{
					displayText += QStringLiteral(" (%1)").arg(authContext->getDidAuthenticateEac1()->getAuthenticatedAuxiliaryData()->getRequiredAge());
				}
			}
			return displayText;
		}

		case OPTIONAL_ROLE:
			return mOptionalRights.contains(right);

		case SELECTED_ROLE:
			return mSelectedRights.contains(right);

		case WRITE_RIGHT:
			return AccessRoleAndRightsUtil::isWriteAccessRight(right);

		default:
			Q_UNREACHABLE();
			return QVariant();
	}
}


QHash<int, QByteArray> ChatModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(NAME_ROLE, "name");
	roles.insert(OPTIONAL_ROLE, "optional");
	roles.insert(SELECTED_ROLE, "selected");
	roles.insert(WRITE_RIGHT, "writeRight");
	return roles;
}


bool ChatModel::setData(const QModelIndex& pIndex, const QVariant& pValue, int pRole)
{
	if (pRole == SELECTED_ROLE)
	{
		auto right = mAllRights.at(pIndex.row());

		if (pValue.toBool()) // is selected?
		{
			if (mSelectedRights.contains(right))
			{
				return false;
			}
			else
			{
				mSelectedRights += right;
			}
		}
		else if (!mSelectedRights.remove(right))
		{
			return false;
		}

		Q_EMIT dataChanged(pIndex, pIndex, QVector<int>({SELECTED_ROLE}));
		return true;
	}

	return false;
}


void ChatModel::transferAccessRights() const
{
	if (auto authContext = mContext.objectCast<AuthContext>())
	{
		Q_ASSERT(authContext->getAccessRightManager());
		*authContext->getAccessRightManager() = mSelectedRights;
	}
}


QSortFilterProxyModel* ChatModel::getFilterOptionalModel()
{
	return &mFilterOptionalModel;
}


QSortFilterProxyModel* ChatModel::getFilterRequiredModel()
{
	return &mFilterRequiredModel;
}


QSortFilterProxyModel* ChatModel::getFilterWriteModel()
{
	return &mFilterWriteModel;
}
