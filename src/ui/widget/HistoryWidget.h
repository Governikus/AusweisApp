/*!
 * \brief Show history entries.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QFileDialog>
#include <QInputDialog>
#include <QPainter>
#include <QScopedPointer>
#include <QTableWidgetItem>
#include <QTextDocument>
#include <QUrl>
#include <QVector>
#include <QWidget>

namespace Ui
{
class HistoryWidget;
} // namespace Ui

namespace governikus
{

class HistoryDetailWidget;

class HistoryWidget
	: public QWidget
{
	Q_OBJECT

	private:
		static const int dateColumn = 0;
		QScopedPointer<Ui::HistoryWidget> mUi;
		HistoryDetailWidget* mHistoryDetailWidget;
		virtual bool eventFilter(QObject* pWatched, QEvent* pEvent) override;
		virtual void paintEvent(QPaintEvent*) override;
		void init();

	private Q_SLOTS:
		void updateTable();

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		HistoryWidget(QWidget* pParent = nullptr);
		virtual ~HistoryWidget() override;
		QWidget* getDetailActivatingWidget() const;

	public Q_SLOTS:
		void onCheckBoxStateChanged(int pState);
		void deleteHistory();
		void exportHistory();
		void searchHistory();
		void onItemClicked(const QModelIndex& pIndex);
};

} // namespace governikus
