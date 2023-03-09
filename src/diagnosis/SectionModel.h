/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
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

	friend class ::test_DiagnosisModel;

	private:
		enum ContentRoles
		{
			TitleRole = Qt::UserRole + 1,
			ContentRole
		};

		QVector<ContentItem> mContentItems;

	public:
		explicit SectionModel(QObject* pParent = nullptr);

		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		void removeAllItems();
		void addContent(const QVector<ContentItem>& pContent);
		[[nodiscard]] QStringList getAsPlaintext(const QString& pPrependString = QString()) const;
};

} // namespace governikus
