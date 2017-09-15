/*!
 * ReaderDriverDialog.h
 *
 * \brief Dialog for detecting attached card readers and
 * suggesting an appropriate driver to be installed.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/ReaderDriverContext.h"
#include "ReaderDriverWidget.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
class ReaderDriverDialog;
}

class QTextCursor;

namespace governikus
{

class ReaderDriverDialog
	: public QDialog
{
	Q_OBJECT

	public:
		ReaderDriverDialog(QWidget* pParent = nullptr);
		virtual ~ReaderDriverDialog();

	Q_SIGNALS:
		void fireUpdateContent();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::ReaderDriverDialog> mUi;
		ReaderDriverWidget* mReaderDriverWidget;
};

} /* namespace governikus */
