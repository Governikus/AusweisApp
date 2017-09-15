/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "HistoryWidget.h"

#include "DeleteHistoryDialog.h"
#include "generic/ListCheckItemWidget.h"
#include "generic/ListItem.h"
#include "generic/ListItemIconLeft.h"
#include "generic/ListItemIconRight.h"
#include "generic/ListItemSubTitle.h"
#include "generic/ListItemTitle.h"
#include "PdfCreator.h"
#include "ui_HistoryWidget.h"

#include <QDebug>
#include <QFormLayout>
#include <QKeyEvent>


using namespace governikus;


HistoryWidget::HistoryWidget(QWidget* pParent)
	: Page(tr("History"), pParent)
	, mUi(new Ui::HistoryWidget())
	, mHistoryDetailWidget(nullptr)
{
	mUi->setupUi(this);

	connect(mUi->historyDeleteButton, &QPushButton::clicked, this, &HistoryWidget::deleteHistory);
	connect(mUi->historySearch, &QLineEdit::textChanged, this, &HistoryWidget::searchHistory);
	connect(mUi->historyExportButton, &QPushButton::clicked, this, &HistoryWidget::exportHistory);
	connect(mUi->saveHistoryCheckBox, &QCheckBox::stateChanged, this, &HistoryWidget::onCheckBoxStateChanged);
	mUi->saveHistoryCheckBox->setChecked(AppSettings::getInstance().getHistorySettings().isEnabled());

	init();
}


HistoryWidget::~HistoryWidget()
{
}


void HistoryWidget::init()
{
	QStringList header;
	header += tr("Date");
	header += tr("Details");

	mUi->historyTableWidget->setColumnCount(header.count());
	mUi->historyTableWidget->setHorizontalHeaderLabels(header);
	mUi->historyTableWidget->verticalHeader()->setVisible(false); //Hide row number
	mUi->historyTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //Not allowed to change content
	mUi->historyTableWidget->setSortingEnabled(false);

	connect(mUi->historyTableWidget, &QTableWidget::doubleClicked, this, &HistoryWidget::onItemClicked);

	mUi->historyTableWidget->installEventFilter(this);

	mUi->noResultWidget->setVisible(false);
}


QWidget* HistoryWidget::getDetailActivatingWidget() const
{
	return mUi->historyTableWidget;
}


void HistoryWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void HistoryWidget::showEvent(QShowEvent* pEvent)
{
	HistorySettings& settings = AppSettings::getInstance().getHistorySettings();
	updateTable(settings.getHistoryEntries());
	mUi->saveHistoryCheckBox->setChecked(settings.isEnabled());

	QWidget::showEvent(pEvent);
}


void HistoryWidget::onCheckBoxStateChanged(int /*pState*/)
{
	AppSettings::getInstance().getHistorySettings().setEnabled(mUi->saveHistoryCheckBox->isChecked());
	AppSettings::getInstance().getHistorySettings().save();
}


void HistoryWidget::updateTable(const QVector<HistoryEntry>& pItems)
{
	mUi->historyTableWidget->clearContents();
	mUi->historyTableWidget->setRowCount(0);

	for (const HistoryEntry& entry : pItems)
	{
		int rowIndex = mUi->historyTableWidget->rowCount();
		mUi->historyTableWidget->insertRow(rowIndex);

		//date column with needed properties
		QLabel* dateLabel = new QLabel(entry.getDateTime().toString(tr("MM/dd/yyyy hh:mm AP")));
		dateLabel->setContentsMargins(11, 11, 11, 11);
		dateLabel->setAlignment(Qt::AlignTop);
		dateLabel->setProperty("termsOfUsage", entry.getTermOfUsage());
		dateLabel->setProperty("date", entry.getDateTime().toString(tr("MM/dd/yyyy hh:mm AP")));

		dateLabel->setFocusPolicy(Qt::TabFocus);
		dateLabel->setAccessibleName(tr("Date:") + entry.getDateTime().toString(tr("MM/dd/yyyy hh:mm AP")));

		mUi->historyTableWidget->setCellWidget(rowIndex, 0, dateLabel);

		//details column with needed properties
		QWidget* centralWidget = new QWidget();

		QFormLayout* centralLayout = new QFormLayout(centralWidget);
		centralLayout->setSpacing(6);
		centralLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

		QLabel* providerLabel = new QLabel(tr("<b>Provider:</b>"));
		providerLabel->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(0, QFormLayout::LabelRole, providerLabel);

		QLabel* providerField = new QLabel(entry.getSubjectName());
		providerField->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(0, QFormLayout::FieldRole, providerField);

		centralWidget->setProperty("provider", entry.getSubjectName());

		QLabel* purposeLabel = new QLabel(tr("<b>Purpose:</b>"));
		purposeLabel->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(1, QFormLayout::LabelRole, purposeLabel);

		QLabel* purposeField = new QLabel(entry.getPurpose());
		purposeField->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(1, QFormLayout::FieldRole, purposeField);

		centralWidget->setProperty("usage", entry.getPurpose());

		QLabel* dataLabel = new QLabel(tr("<b>Data:</b>"));
		dataLabel->setFocusPolicy(Qt::TabFocus);
		dataLabel->setAlignment(Qt::AlignTop);
		centralLayout->setWidget(2, QFormLayout::LabelRole, dataLabel);

		QLabel* requestedDataLabel = new QLabel(entry.getRequestedData().trimmed());
		requestedDataLabel->setWordWrap(true);
		requestedDataLabel->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(2, QFormLayout::FieldRole, requestedDataLabel);

		centralWidget->setProperty("requestedData", entry.getRequestedData());

		mUi->historyTableWidget->setCellWidget(rowIndex, 1, centralWidget);
	}

	mUi->historyTableWidget->resizeRowsToContents();
	mUi->historyTableWidget->sortByColumn(dateColumn); // Sort by date
}


bool HistoryWidget::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* pressed = static_cast<QKeyEvent*>(pEvent);
		if ((pressed->key() == Qt::Key_Enter) || (pressed->key() == Qt::Key_Return) || (pressed->key() == Qt::Key_Space))
		{
			const auto selectedIndexes = mUi->historyTableWidget->selectionModel()->selectedIndexes();
			for (auto index : selectedIndexes)
			{
				QWidget* tmpWidget = qobject_cast<QWidget*>(mUi->historyTableWidget->cellWidget(index.row(), dateColumn));
				DetailDialog d(this);
				d.setDetails(tmpWidget->property("termsOfUsage").toString());
				d.exec();
			}
			return true;
		}
	}

	return QWidget::eventFilter(pObject, pEvent);
}


void HistoryWidget::deleteHistory()
{
	DeleteHistoryDialog* deleteHistoryDialog = new DeleteHistoryDialog(this);
	if (deleteHistoryDialog->exec() == QDialog::Rejected)
	{
		return;
	}

	QDateTime latestToKeep = QDateTime::currentDateTime();
	switch (deleteHistoryDialog->getTimePeriod())
	{
		case TimePeriod::PAST_HOUR:
			latestToKeep = latestToKeep.addSecs(-60 * 60);
			break;

		case TimePeriod::PAST_DAY:
			latestToKeep = latestToKeep.addDays(-1);
			break;

		case TimePeriod::PAST_WEEK:
			latestToKeep = latestToKeep.addDays(-7);
			break;

		case TimePeriod::LAST_FOUR_WEEKS:
			latestToKeep = latestToKeep.addDays(-7 * 4);
			break;

		case TimePeriod::ALL_HISTORY:
			latestToKeep = QDateTime();
			break;
	}

	HistorySettings& settings = AppSettings::getInstance().getHistorySettings();
	settings.deleteSettings(latestToKeep);
	settings.save();
	updateTable(settings.getHistoryEntries());
}


void HistoryWidget::exportHistory()
{
	QString pdfName = QFileDialog::getSaveFileName(this,
			QApplication::applicationName() + " - " + tr("Save history"),
			QDir::currentPath(),
			tr("PDF Documents (*.pdf)"));

	PdfExport::exportHistory(pdfName);
}


void HistoryWidget::searchHistory()
{
	mUi->historyTableWidget->setVisible(true);
	mUi->noResultWidget->setVisible(false);

	bool anyMatch = false;

	for (int i = 0; i < mUi->historyTableWidget->rowCount(); ++i)
	{
		bool match = false;
		for (int j = 0; j < mUi->historyTableWidget->columnCount(); ++j)
		{
			QString tmpData;
			QWidget* tmpWidget = qobject_cast<QWidget*>(mUi->historyTableWidget->cellWidget(i, j));
			if (j == dateColumn)
			{
				tmpData = tmpWidget->property("date").toString();
			}
			else
			{
				tmpData = tmpWidget->property("provider").toString() + " " + tmpWidget->property("usage").toString() + " " + tmpWidget->property("requestedData").toString();
			}

			if (tmpData.contains(mUi->historySearch->text(), Qt::CaseInsensitive))
			{
				match = true;
				anyMatch = true;
				break;
			}
		}
		mUi->historyTableWidget->setRowHidden(i, !match);

	}

	mUi->historyTableWidget->setVisible(anyMatch);
	mUi->noResultWidget->setVisible(!anyMatch);
}


void HistoryWidget::onItemClicked(const QModelIndex& pIndex)
{
	QModelIndex idx = pIndex;

	qDebug() << "selected model index: " << idx;

	if (pIndex.isValid())
	{
		QWidget* tmpWidget = qobject_cast<QWidget*>(mUi->historyTableWidget->cellWidget(idx.row(), dateColumn));
		DetailDialog d(this);
		d.setDetails(tmpWidget->property("termsOfUsage").toString());
		d.exec();
	}
}
