/*!
 * \brief Model implementation for the chat.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ChatModel.h"

#include "asn1/AccessRoleAndRight.h"
#include "asn1/CVCertificate.h"
#include "AppSettings.h"
#include "context/SelfAuthContext.h"


using namespace governikus;


ChatModel::ChatModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mAuthContext()
	, mAllRights()
	, mOptionalRights()
	, mSelectedRights()
	, mFilterOptionalModel()
	, mFilterRequiredModel()
{
	resetContext(QSharedPointer<AuthContext>());

	initFilterModel(mFilterOptionalModel, QStringLiteral("true"));
	initFilterModel(mFilterRequiredModel, QStringLiteral("false"));

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireSettingsChanged, this, [this]()
			{
				beginResetModel();
				endResetModel();
			});
}


void ChatModel::initFilterModel(QSortFilterProxyModel& pModel, const QString& pFilter)
{
	pModel.setSourceModel(this);
	pModel.setFilterRole(ChatRoles::OPTIONAL_ROLE);
	pModel.setFilterRegExp(pFilter);
}


void ChatModel::resetContext(const QSharedPointer<AuthContext>& pContext)
{
	mAuthContext = pContext;

	if (pContext.objectCast<SelfAuthContext>())
	{
		/* nothing to do, access rights are static */
	}
	else if (!pContext.isNull() /* it's an AuthContext */)
	{
		beginResetModel();

		mAllRights.clear();
		mOptionalRights.clear();
		mSelectedRights.clear();

		endResetModel();

		connect(mAuthContext.data(), &AuthContext::fireAuthenticationDataChanged, this, &ChatModel::onAuthenticationDataChanged);
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

		endResetModel();
	}
}


void ChatModel::onAuthenticationDataChanged()
{
	beginResetModel();

	mAllRights.clear();
	mOptionalRights.clear();
	mSelectedRights.clear();

	if (!mAuthContext->getRequiredAccessRights().isEmpty())
	{
		setOrderedAllRights(mAuthContext->getRequiredAccessRights());
		mSelectedRights += mAuthContext->getRequiredAccessRights();
	}

	if (!mAuthContext->getOptionalAccessRights().isEmpty())
	{
		mOptionalRights += mAuthContext->getOptionalAccessRights();
		setOrderedAllRights(mAuthContext->getOptionalAccessRights());
		mSelectedRights += mAuthContext->getOptionalAccessRights();
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
	return mAllRights.size();
}


QVariant ChatModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto right = mAllRights.at(pIndex.row());
		if (pRole == Qt::DisplayRole || pRole == NAME_ROLE)
		{
			QString displayText = AccessRoleAndRightsUtil::toDisplayText(right);
			if (right == AccessRight::AGE_VERIFICATION)
			{
				displayText += QStringLiteral(" (%1)").arg(mAuthContext->getRequiredAge());
			}
			return displayText;
		}
		if (pRole == OPTIONAL_ROLE)
		{
			return mOptionalRights.contains(right);
		}
		if (pRole == SELECTED_ROLE)
		{
			return mSelectedRights.contains(right);
		}
	}
	return QVariant();
}


QHash<int, QByteArray> ChatModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(NAME_ROLE, "name");
	roles.insert(OPTIONAL_ROLE, "optional");
	roles.insert(SELECTED_ROLE, "selected");
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
