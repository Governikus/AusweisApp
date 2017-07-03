/*!
 * DiagnosisWidget.h
 *
 * \brief Widget for display the diagnosis information.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QScopedPointer>
#include <QTextCharFormat>
#include <QTreeWidgetItem>
#include <QWidget>

#include "context/DiagnosisContext.h"
#include "generic/Page.h"

namespace Ui
{
class DiagnosisWidget;
}

class QTextCursor;

namespace governikus
{

class DiagnosisWidget
	: public Page
{
	Q_OBJECT

	public:
		DiagnosisWidget(DiagnosisContext* pContext, QWidget* pParent = nullptr);
		virtual ~DiagnosisWidget();

		QString getInfoTextEdit();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

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
};

} /* namespace governikus */
