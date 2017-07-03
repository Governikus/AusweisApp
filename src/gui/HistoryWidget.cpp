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
	header.append(tr("Date"));
	header.append(tr("Details"));

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
		QLabel* dateLabel = new QLabel(entry.getDateTime().toString(QStringLiteral("dd.MM.yyyy hh:mm")));
		dateLabel->setContentsMargins(11, 11, 11, 11);
		dateLabel->setAlignment(Qt::AlignTop);
		dateLabel->setProperty("termsOfUsage", entry.getTermOfUsage());
		dateLabel->setProperty("date", entry.getDateTime().toString(QStringLiteral("dd.MM.yyyy hh:mm")));

		dateLabel->setFocusPolicy(Qt::TabFocus);
		dateLabel->setAccessibleName(tr("Date:") + entry.getDateTime().toString(QStringLiteral("dd.MM.yyyy hh:mm")));

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


bool HistoryWidget::eventFilter(QObject*, QEvent* pEvent)
{
	switch (pEvent->type())
	{
		case QEvent::KeyPress:
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
		break;

		default:
			return false;
	}

	return false;
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
	QString htmlContent, company_header, company_footer;
	QPrinter printer;
	QRect printer_rect(printer.pageRect());

	QString pdfName = QFileDialog::getSaveFileName(this,
			QApplication::applicationName() + " - " + tr("Save history"),
			QDir::currentPath(),
			tr("PDF Documents (*.pdf)"));

	if (pdfName.size() > 0)
	{
		printer.setOutputFileName(pdfName);

		//Header

		//Header
		QDateTime dateTime = QDateTime::currentDateTime();
		QString date = dateTime.toString(QStringLiteral("dd.MM.yyyy"));
		QString time = dateTime.toString(QStringLiteral("hh:mm"));

		company_header.append("<br><br><table cellspacing='5' cellpadding='5' width='100%'>");
		company_header.append("<tr>");
		company_header.append(tr("<td valign='middle'><h1>History - ") + QApplication::applicationName() + tr("</h1><h3>AusweisApp2 is a product of Governikus GmbH & Co. KG - on behalf of the Bundesministerium des Innern (Federal Ministry of the Interior).</h3></td>"));
		company_header.append("<th rowspan='2'><p align='center'><img src=':/images/information.png'></p></th>");
		company_header.append("</tr>");
		company_header.append("<tr>");

		company_header.append(tr("<td valign='middle'><br><h3>At %1 %2 the following data were saved:").arg(date, time) + "</h3></td>");
		company_header.append("</tr>");
		company_header.append("</table>");

		company_footer.append(tr("<h3>For further information, please see <a href='https://www.ausweisapp.bund.de/'>https://www.ausweisapp.bund.de/</a></h3>"));

		//Setting up the header and calculating the header size
		QTextDocument* document_header = new QTextDocument(this);
		document_header->setPageSize(printer_rect.size());
		document_header->setHtml(company_header);
		QSizeF header_size = document_header->size();


		//Setting up the footer and calculating the footer size
		QTextDocument* document_footer = new QTextDocument(this);
		document_footer->setPageSize(printer_rect.size());
		document_footer->setHtml(company_footer);
		QSizeF footer_size = document_footer->size();

		//Calculating the main document size for one page
		QSizeF center_size(printer_rect.width(),
				(printer.pageRect().height() - header_size.toSize().height() - footer_size.toSize().height()));

		htmlContent.append("<table width='100%' cellspacing='3' cellpadding='5'>");
		htmlContent.append(tr("<tr><td><b>Date</b></td><td><b>Details</b></td></tr>"));

		auto history = AppSettings::getInstance().getHistorySettings().getHistoryEntries();

		for (int i = 0; i < history.size(); ++i)
		{
			if (i % 2 == 0)
			{
				htmlContent.append("<tr>");
			}
			else
			{
				htmlContent.append("<tr style='background-color:#f6f6f6'>");
			}
			htmlContent.append("<td>");
			htmlContent.append(history.at(i).getDateTime().toString(QStringLiteral("dd.MM.yyyy hh:mm")));
			htmlContent.append("</td>");
			htmlContent.append("<td>");

			htmlContent.append("<table><tr>");
			htmlContent.append(tr("<td>Provider:</td>"));
			htmlContent.append("<td>" + history.at(i).getSubjectName() + "</td>");
			htmlContent.append("</tr><tr>");
			htmlContent.append(tr("<td>Purpose:</td>"));
			htmlContent.append("<td>" + history.at(i).getPurpose() + "</td>");
			htmlContent.append("</tr><tr>");
			htmlContent.append(tr("<td>Data:</td>"));
			htmlContent.append("<td>" + history.at(i).getRequestedData() + "</td>");
			htmlContent.append("</tr></table>");

			htmlContent.append("</td></tr>");
		}

		htmlContent.append("</table>");

		//Insert HTML in main document
		QTextDocument* main_doc = new QTextDocument(this);
		main_doc->setHtml(htmlContent);
		main_doc->setPageSize(center_size);

		//Setting up the rectangles for each section.
		QRect headerRect = QRect(QPoint(0, 0), document_header->size().toSize());
		QRect footerRect = QRect(QPoint(0, 0), document_footer->size().toSize());
		QRect contentRect = QRect(QPoint(0, 0), main_doc->size().toSize()); // Main content rectangle.
		QRect currentRect = QRect(QPoint(0, 0), center_size.toSize()); // Current main content rectangle.

		QPainter painter(&printer);

		while (currentRect.intersects(contentRect))
		{ //Loop if the current content rectangle intersects with the main content rectangle.
			//Resetting the painter matrix co-ordinate system.
			painter.resetMatrix();
			//Applying negative translation of painter co-ordinate system by current main content rectangle top y coordinate.
			painter.translate(0, -currentRect.y());
			//Applying positive translation of painter co-ordinate system by header hight.
			painter.translate(0, headerRect.height());
			//Drawing the center content for current page.
			main_doc->drawContents(&painter, currentRect);
			//Resetting the painter matrix co ordinate system.
			painter.resetMatrix();
			//Drawing the header on the top of the page
			document_header->drawContents(&painter, headerRect);
			//Applying positive translation of painter co-ordinate system to draw the footer
			painter.translate(0, headerRect.height());
			painter.translate(0, center_size.height());
			document_footer->drawContents(&painter, footerRect);

			//Translating the current rectangle to the area to be printed for the next page
			currentRect.translate(0, currentRect.height());
			//Inserting a new page if there is till area left to be printed
			if (currentRect.intersects(contentRect))
			{
				printer.newPage();
			}
		}
		QDesktopServices::openUrl(QUrl(pdfName));
	}
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
