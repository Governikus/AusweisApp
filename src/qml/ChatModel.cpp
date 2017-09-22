/*!
 * \brief Model implementation for the chat.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "asn1/AccessRoleAndRight.h"
#include "asn1/CVCertificate.h"
#include "AppSettings.h"
#include "context/SelfAuthenticationContext.h"
#include "ChatModel.h"

using namespace governikus;


ChatModel::ChatModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mAuthContext()
	, mAllRights()
	, mOptionalRights()
	, mSelectedRights()
	, mFilterOptionalModel()
	, mFilterRequiredModel()
	, mRequiredAge()
{
	resetContext(QSharedPointer<AuthContext>());

	initFilterModel(mFilterOptionalModel, QStringLiteral("true"));
	initFilterModel(mFilterRequiredModel, QStringLiteral("false"));
}


void ChatModel::initFilterModel(QSortFilterProxyModel& pModel, const QString& pFilter)
{
	pModel.setSourceModel(this);
	pModel.setFilterRole(ChatRoles::OPTIONAL);
	pModel.setFilterRegExp(pFilter);
}


void ChatModel::resetContext(QSharedPointer<AuthContext> pContext)
{
	mAuthContext = pContext;

	if (pContext.dynamicCast<SelfAuthenticationContext>())
	{
		/* nothing to do, access rights are static */
	}
	else if (!pContext.isNull() /* it's an AuthContext */)
	{
		beginResetModel();

		mAllRights.clear();
		mOptionalRights.clear();
		mSelectedRights.clear();
		mRequiredAge.clear();

		endResetModel();

		connect(mAuthContext.data(), &AuthContext::fireOptionalChatChanged, this, &ChatModel::onOptionalChatChanged);
		connect(mAuthContext.data(), &AuthContext::fireRequiredChatChanged, this, &ChatModel::onRequiredChatChanged);
		connect(mAuthContext.data(), &AuthContext::fireRequiredAgeChanged, this, &ChatModel::onRequiredAgeChanged);
	}
	else
	{
		/* set static access rights according to selfAuthentication*/
		Q_ASSERT(pContext.isNull());

		beginResetModel();

		mAllRights.clear();
		mAllRights += AccessRight::READ_DG05;
		mAllRights += AccessRight::READ_DG13;
		mAllRights += AccessRight::READ_DG04;
		mAllRights += AccessRight::READ_DG07;
		mAllRights += AccessRight::READ_DG08;
		mAllRights += AccessRight::READ_DG09;
		mAllRights += AccessRight::READ_DG17;
		mAllRights += AccessRight::READ_DG01;
		mAllRights += AccessRight::READ_DG10;
		mAllRights += AccessRight::READ_DG06;
		mAllRights += AccessRight::READ_DG02;
		mAllRights += AccessRight::READ_DG19;

		mOptionalRights.clear();
		mSelectedRights = mAllRights.toSet();
		mRequiredAge.clear();

		endResetModel();
	}
}


void ChatModel::onOptionalChatChanged()
{
	beginResetModel();

	Q_ASSERT(mAuthContext->getOptionalChat());
	mOptionalRights += mAuthContext->getOptionalChat()->getAccessRights();
	setOrderedAllRights(mAuthContext->getOptionalChat()->getAccessRights());
	mSelectedRights += mAuthContext->getOptionalChat()->getAccessRights();

	endResetModel();
}


void ChatModel::onRequiredChatChanged()
{
	beginResetModel();

	Q_ASSERT(mAuthContext->getRequiredChat());
	setOrderedAllRights(mAuthContext->getRequiredChat()->getAccessRights());
	mSelectedRights += mAuthContext->getRequiredChat()->getAccessRights();

	endResetModel();
}


void ChatModel::onRequiredAgeChanged()
{
	beginResetModel();
	mRequiredAge = mAuthContext->getRequiredAge();
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
	return mAllRights.size();
}


QVariant ChatModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto right = mAllRights.at(pIndex.row());
		if (pRole == Qt::DisplayRole || pRole == NAME)
		{
			QString displayText = AccessRoleAndRightsUtil::toDisplayText(right);
			if (right == AccessRight::AGE_VERIFICATION && !mRequiredAge.isEmpty())
			{
				displayText = displayText.append(" (%1)").arg(mRequiredAge);
			}
			return displayText;
		}
		if (pRole == OPTIONAL)
		{
			return mOptionalRights.contains(right);
		}
		if (pRole == SELECTED)
		{
			return mSelectedRights.contains(right);
		}
	}
	return QVariant();
}


QHash<int, QByteArray> ChatModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(NAME, "name");
	roles.insert(OPTIONAL, "optional");
	roles.insert(SELECTED, "selected");
	return roles;
}


bool ChatModel::setData(const QModelIndex& pIndex, const QVariant& pValue, int pRole)
{
	if (pRole == SELECTED)
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

		dataChanged(pIndex, pIndex, QVector<int>({SELECTED}));
		return true;
	}

	return false;
}


void ChatModel::transferAccessRights()
{
	Q_ASSERT(mAuthContext);

	mAuthContext->setEffectiveAccessRights(mSelectedRights);
}


QSortFilterProxyModel* ChatModel::getFilterOptionalModel()
{
	return &mFilterOptionalModel;
}


QSortFilterProxyModel* ChatModel::getFilterRequiredModel()
{
	return &mFilterRequiredModel;
}
