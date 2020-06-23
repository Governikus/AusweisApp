/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QVector>


namespace governikus
{

class DiagnosisItem
	: public QObject
	, public QEnableSharedFromThis<DiagnosisItem>
{
	Q_OBJECT

	private:
		QString mText;
		QVector<QSharedPointer<DiagnosisItem>> mChildren;
		QSharedPointer<DiagnosisItem> mParent;

		int getIndexOf(const DiagnosisItem* const pChild) const;
		void setParent(const QSharedPointer<DiagnosisItem>& pParent);

	public:
		explicit DiagnosisItem(const QString& pText);

		void addChild(const QSharedPointer<DiagnosisItem>& pChild);
		const QString& getText() const;

		int childCount() const;
		const QSharedPointer<DiagnosisItem> getChild(int pRow) const;
		void clearChildren();
		QSharedPointer<DiagnosisItem> parentItem();
		int row() const;
		void appendPlaintextContent(QStringList& pOutput, const QString& pPrefix = QString());
};

} // namespace governikus
