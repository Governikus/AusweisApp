/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ContentItem.h"

#include <QAbstractListModel>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVector>

class test_DiagnosisModel;


namespace governikus
{

class SectionModel
	: public QAbstractListModel
{
	Q_OBJECT

	enum ContentRoles
	{
		TitleRole = Qt::UserRole + 1,
		ContentRole
	};

	private:
		friend class ::test_DiagnosisModel;
		QVector<QSharedPointer<ContentItem>> mContentItems;

	public:
		explicit SectionModel(QObject* pParent = nullptr);

		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		void addItem(const QString& pTitle, const QString& pContent);
		void addItem(const QSharedPointer<ContentItem>& pContentItem);
		void addItemWithoutTitle(const QString& pContent);
		void addTitleWithoutContent(const QString& pTitle);
		[[nodiscard]] bool contains(const QSharedPointer<ContentItem>& pContentItem) const;
		void removeAllItems();
		void emitDataChangedForItem(const QSharedPointer<ContentItem>& pItem);
		void replaceWithSections(QVector<QSharedPointer<SectionModel>> pSections);
		[[nodiscard]] QStringList getAsPlaintext(const QString& pPrependString = QString()) const;
};

} // namespace governikus
