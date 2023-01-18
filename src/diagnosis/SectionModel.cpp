/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
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

	QSharedPointer<ContentItem> item = mContentItems.at(row);
	switch (pRole)
	{
		case Qt::DisplayRole:
			if (item->mTitle.isEmpty())
			{
				return item->mContent;
			}
			else
			{
				return item->mTitle + endl + item->mContent;
			}

		case TitleRole:
			return item->mTitle;

		case ContentRole:
			return item->mContent;

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


void SectionModel::addItem(const QString& pTitle, const QString& pContent)
{
	addItem(QSharedPointer<ContentItem>::create(pTitle, pContent));
}


void SectionModel::addItem(const QSharedPointer<ContentItem>& pContentItem)
{
	beginInsertRows(index(0), mContentItems.size(), mContentItems.size());
	mContentItems.append(pContentItem);
	endInsertRows();
}


void SectionModel::addItemWithoutTitle(const QString& pContent)
{
	addItem(QString(), pContent);
}


void SectionModel::addTitleWithoutContent(const QString& pTitle)
{
	addItem(pTitle, QString());
}


bool SectionModel::contains(const QSharedPointer<ContentItem>& pContentItem) const
{
	return mContentItems.contains(pContentItem);
}


void SectionModel::removeAllItems()
{
	if (!mContentItems.empty())
	{
		beginRemoveRows(index(0), 0, mContentItems.size() - 1);
		mContentItems.clear();
		endRemoveRows();
	}
}


void SectionModel::emitDataChangedForItem(const QSharedPointer<ContentItem>& pItem)
{
	QModelIndex itemIndex = index(mContentItems.indexOf(pItem));
	Q_EMIT dataChanged(itemIndex, itemIndex);
}


void SectionModel::replaceWithSections(QVector<QSharedPointer<SectionModel>> pSections)
{
	beginResetModel();
	removeAllItems();

	for (const auto& section : std::as_const(pSections))
	{
		const auto& sectionItems = section->mContentItems;
		for (const auto& item : sectionItems)
		{
			addItem(item);
		}
	}

	endResetModel();
}


QStringList SectionModel::getAsPlaintext(const QString& pPrependString) const
{
	QStringList sectionPlaintext;
	for (const auto& item : std::as_const(mContentItems))
	{
		if (!item->mTitle.isEmpty())
		{
			sectionPlaintext << pPrependString + item->mTitle;
		}

		if (!item->mContent.isEmpty())
		{
			sectionPlaintext << pPrependString + item->mContent;
		}
	}

	return sectionPlaintext;
}
