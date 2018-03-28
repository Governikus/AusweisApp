/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthenticationModel.h"

#include "context/SelfAuthContext.h"
#include "LanguageLoader.h"

using namespace governikus;


void SelfAuthenticationModel::onSelfAuthenticationDataChanged()
{
	beginResetModel();
	mSelfData.clear();

	if (mContext && mContext->getSelfAuthenticationData().isValid())
	{
		const auto& selfdata = mContext->getSelfAuthenticationData().getOrderedSelfData();
		for (const auto& entry : selfdata)
		{
			if (entry.first.isEmpty())
			{
				Q_ASSERT(!mSelfData.isEmpty());
				const auto& previous = mSelfData.takeLast();
				mSelfData << qMakePair(previous.first, previous.second + QStringLiteral("<br/>") + entry.second);
			}
			else
			{
				mSelfData << entry;
			}
		}
	}

	endResetModel();
}


SelfAuthenticationModel::SelfAuthenticationModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mContext()
	, mSelfData()
{
	onSelfAuthenticationDataChanged();
}


void SelfAuthenticationModel::resetContext(const QSharedPointer<SelfAuthContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &SelfAuthContext::fireSelfAuthenticationDataChanged, this, &SelfAuthenticationModel::onSelfAuthenticationDataChanged);
	}
	onSelfAuthenticationDataChanged();
}


void SelfAuthenticationModel::startWorkflow()
{
	Q_EMIT fireStartWorkflow();
}


void SelfAuthenticationModel::cancelWorkflow()
{
	if (mContext)
	{
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


bool SelfAuthenticationModel::isBasicReader()
{
	if (mContext)
	{
		return mContext->getCardConnection()->getReaderInfo().isBasicReader();
	}

	return true;
}


int SelfAuthenticationModel::rowCount(const QModelIndex&) const
{
	return mSelfData.size();
}


QVariant SelfAuthenticationModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto pair = mSelfData.at(pIndex.row());
		if (pRole == Qt::DisplayRole || pRole == NAME)
		{
			return pair.first;
		}
		if (pRole == VALUE)
		{
			return pair.second;
		}
	}
	return QVariant();
}


QHash<int, QByteArray> SelfAuthenticationModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(NAME, "name");
	roles.insert(VALUE, "value");
	return roles;
}


bool SelfAuthenticationModel::event(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		onSelfAuthenticationDataChanged();
	}
	return QAbstractListModel::event(pEvent);
}
