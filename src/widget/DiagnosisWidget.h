/*!
 * \brief Widget for display the diagnosis information.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QTextCharFormat>
#include <QTreeWidgetItem>
#include <QWidget>

#include "context/DiagnosisContext.h"

namespace Ui
{
class DiagnosisWidget;
}

class QTextCursor;

namespace governikus
{

class DiagnosisWidget
	: public QWidget
{
	Q_OBJECT

	private:
		class Field;

	private Q_SLOTS:
		void onPcscInfoChanged();
		void onReaderInfosChanged();
		void onTimestampChanged();

	private:
		Field* insertField(QTextCursor& pCursor, const QString& pHeading, bool pIsFirstField = false);
		void setFieldText(const QScopedPointer<Field>& pField, const QString& pText);

		void endBlockAndResetFormat(QTextCursor& pCursor);
		void insertComponentList(QTextCursor& pCursor, const QString& pTitle,
				const QVector<DiagnosisContext::ComponentInfo>& pComponents, QTreeWidgetItem* pParentTreeWidgetItem);

		void deleteItem(QTreeWidgetItem* pItem);

	private:
		QScopedPointer<Ui::DiagnosisWidget> mUi;
		DiagnosisContext* mContext;
		QScopedPointer<Field> mOsField;
		QScopedPointer<Field> mAppVersionField;
		QScopedPointer<Field> mPcscField;
		QScopedPointer<Field> mReadersField;
		QScopedPointer<Field> mTimestampField;
		QTextCharFormat mPlainTextFormat;
		QTextCharFormat mHeadingTextFormat;
		QTextBlockFormat mBasicBlockFormat;

		QTreeWidgetItem* mOsVersionTreeItem;
		QTreeWidgetItem* mAppVersionTreeItem;
		QTreeWidgetItem* mReadersTreeItem;
		QTreeWidgetItem* mPcscTreeItem;
		QTreeWidgetItem* mReaderWaitItem;
		QTreeWidgetItem* mPcscWaitItem;
		QTreeWidgetItem* mTimestampItem;

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		DiagnosisWidget(DiagnosisContext* pContext, QWidget* pParent = nullptr);
		virtual ~DiagnosisWidget() override;

		QString getInfoTextEdit();
};

} /* namespace governikus */
