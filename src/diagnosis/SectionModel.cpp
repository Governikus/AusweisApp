/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SectionModel.h"


using namespace governikus;


SectionModel::SectionModel(QObject* pParent)
	: QAbstractListModel(pParent)
{

}


QVariant SectionModel::data(const QModelIndex& pIndex, int pRole) const
{
	const int row = pIndex.row();
	if (!pIndex.isValid() || row >= mContentItems.size())
	{
		return QVariant();
	}

	static const QLatin1Char endl('\n');

	const ContentItem& item = mContentItems.at(row);
	switch (pRole)
	{
		case Qt::DisplayRole:
			if (item.mTitle.isEmpty())
			{
				return item.mContent;
			}
			else
			{
				return item.mTitle + endl + item.mContent;
			}

		case TitleRole:
			return item.mTitle;

		case ContentRole:
			return item.mContent;

		default:
			return QVariant();
	}
}


int SectionModel::rowCount(const QModelIndex& pParent) const
{
	Q_UNUSED(pParent)
	return mContentItems.size();
}


QHash<int, QByteArray> SectionModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
	roles.insert(TitleRole, QByteArrayLiteral("title"));
	roles.insert(ContentRole, QByteArrayLiteral("content"));
	return roles;
}


void SectionModel::removeAllItems()
{
	if (mContentItems.empty())
	{
		return;
	}

	beginResetModel();
	mContentItems.clear();
	endResetModel();
}


void SectionModel::addContent(const QVector<ContentItem>& pContent)
{
	if (pContent.empty())
	{
		return;
	}

	beginInsertRows(index(mContentItems.size()), mContentItems.size(), mContentItems.size() + pContent.size() - 1);
	mContentItems << pContent;
	endInsertRows();
}


QStringList SectionModel::getAsPlaintext(const QString& pPrependString) const
{
	QStringList sectionPlaintext;
	for (const auto& item : std::as_const(mContentItems))
	{
		if (!item.mTitle.isEmpty())
		{
			sectionPlaintext << pPrependString + item.mTitle;
		}

		if (!item.mContent.isEmpty())
		{
			sectionPlaintext << pPrependString + item.mContent;
		}
	}

	return sectionPlaintext;
}
