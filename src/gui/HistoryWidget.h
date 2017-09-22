/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppSettings.h"
#include "DetailDialog.h"
#include "generic/ListCheckItemWidget.h"
#include "generic/Page.h"
#include "HistoryDetailWidget.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QPainter>
#include <QScopedPointer>
#include <QtPrintSupport/QPrinter>
#include <QTableWidgetItem>
#include <QTextDocument>
#include <QUrl>
#include <QVector>

namespace Ui
{
class HistoryWidget;
}

namespace governikus
{

class ProviderDetailWidget;

class HistoryWidget
	: public Page
{
	Q_OBJECT

	public:
		HistoryWidget(QWidget* pParent = nullptr);
		virtual ~HistoryWidget();

		QWidget* getDetailActivatingWidget() const;

	public Q_SLOTS:
		virtual void showEvent(QShowEvent*) override;
		void onCheckBoxStateChanged(int pState);
		void deleteHistory();
		void exportHistory();
		void searchHistory();
		void onItemClicked(const QModelIndex& pIndex);

	private:
	private:
		static const int dateColumn = 0;

		QScopedPointer<Ui::HistoryWidget> mUi;
		HistoryDetailWidget* mHistoryDetailWidget;

	private:
		virtual bool eventFilter(QObject* pWatched, QEvent* pEvent) override;
		virtual void paintEvent(QPaintEvent*) override;
		void updateTable(const QVector<HistoryEntry>& pItems);
		void init();
};

} /* namespace governikus */
