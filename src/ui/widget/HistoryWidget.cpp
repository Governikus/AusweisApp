/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryWidget.h"
#include "ui_HistoryWidget.h"

#include "asn1/AccessRoleAndRight.h"
#include "AppSettings.h"
#include "DeleteHistoryDialog.h"
#include "DetailDialog.h"
#include "generic/ListCheckItemWidget.h"
#include "generic/ListItem.h"
#include "generic/ListItemIconLeft.h"
#include "generic/ListItemIconRight.h"
#include "generic/ListItemSubTitle.h"
#include "generic/ListItemTitle.h"
#include "LanguageLoader.h"
#include "PdfExporter.h"
#include "ScopeGuard.h"

#include <QDateTime>
#include <QDebug>
#include <QFormLayout>
#include <QKeyEvent>

using namespace governikus;


HistoryWidget::HistoryWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::HistoryWidget())
	, mHistoryDetailWidget(nullptr)
{
	mUi->setupUi(this);

	connect(mUi->historyDeleteButton, &QPushButton::clicked, this, &HistoryWidget::deleteHistory);
	connect(mUi->historySearch, &QLineEdit::textChanged, this, &HistoryWidget::searchHistory);
	connect(mUi->historyExportButton, &QPushButton::clicked, this, &HistoryWidget::exportHistory);
	connect(mUi->historyTableWidget, &QTableWidget::doubleClicked, this, &HistoryWidget::onItemClicked);
	connect(mUi->saveHistoryCheckBox, &QCheckBox::stateChanged, this, &HistoryWidget::onCheckBoxStateChanged);
	mUi->saveHistoryCheckBox->setChecked(Env::getSingleton<AppSettings>()->getHistorySettings().isEnabled());

	connect(&Env::getSingleton<AppSettings>()->getHistorySettings(), &HistorySettings::fireHistoryInfosChanged, this, &HistoryWidget::updateTable);
	connect(&Env::getSingleton<AppSettings>()->getHistorySettings(), &HistorySettings::fireEnabledChanged, this, [this](bool pValue){
				mUi->saveHistoryCheckBox->setChecked(pValue);
			});

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
	mUi->historyTableWidget->installEventFilter(this);

	mUi->noResultWidget->setVisible(false);
	updateTable();
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


void HistoryWidget::onCheckBoxStateChanged(int /*pState*/)
{
	auto& historySettings = Env::getSingleton<AppSettings>()->getHistorySettings();
	historySettings.setEnabled(mUi->saveHistoryCheckBox->isChecked());
	historySettings.save();
}


void HistoryWidget::updateTable()
{
	const auto& items = Env::getSingleton<AppSettings>()->getHistorySettings().getHistoryInfos();

	const ScopeGuard guard([this] {
				mUi->historyTableWidget->setUpdatesEnabled(true);
			});

	mUi->historyTableWidget->setUpdatesEnabled(false);
	mUi->historyTableWidget->clearContents();
	mUi->historyTableWidget->setRowCount(0);

	for (const HistoryInfo& info : items)
	{
		int rowIndex = mUi->historyTableWidget->rowCount();
		mUi->historyTableWidget->insertRow(rowIndex);

		//date column with needed properties
		const auto& dateTime = LanguageLoader::getInstance().getUsedLocale().toString(info.getDateTime(), tr("dd.MM.yyyy hh:mm AP"));
		QLabel* dateLabel = new QLabel(dateTime);
		dateLabel->setContentsMargins(11, 11, 11, 11);
		dateLabel->setAlignment(Qt::AlignTop);
		dateLabel->setProperty("termsOfUsage", info.getTermOfUsage());
		dateLabel->setProperty("date", dateTime);

		dateLabel->setFocusPolicy(Qt::TabFocus);
		dateLabel->setAccessibleName(tr("Date:") + dateTime);

		mUi->historyTableWidget->setCellWidget(rowIndex, 0, dateLabel);

		//details column with needed properties
		QWidget* centralWidget = new QWidget();

		QFormLayout* centralLayout = new QFormLayout(centralWidget);
		centralLayout->setSpacing(6);
		centralLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

		const auto& bold = QStringLiteral("<b>%1:</b>");
		QLabel* providerLabel = new QLabel(bold.arg(tr("Provider")));
		providerLabel->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(0, QFormLayout::LabelRole, providerLabel);

		QLabel* providerField = new QLabel(info.getSubjectName());
		providerField->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(0, QFormLayout::FieldRole, providerField);

		centralWidget->setProperty("provider", info.getSubjectName());

		QLabel* purposeLabel = new QLabel(bold.arg(tr("Purpose")));
		purposeLabel->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(1, QFormLayout::LabelRole, purposeLabel);

		QLabel* purposeField = new QLabel(info.getPurpose());
		purposeField->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(1, QFormLayout::FieldRole, purposeField);

		centralWidget->setProperty("usage", info.getPurpose());

		QLabel* dataLabel = new QLabel(bold.arg(tr("Data")));
		dataLabel->setFocusPolicy(Qt::TabFocus);
		dataLabel->setAlignment(Qt::AlignTop);
		centralLayout->setWidget(2, QFormLayout::LabelRole, dataLabel);

		const auto& requestedData = AccessRoleAndRightsUtil::joinFromTechnicalName(info.getRequestedData());
		QLabel* requestedDataLabel = new QLabel(requestedData);
		requestedDataLabel->setWordWrap(true);
		requestedDataLabel->setFocusPolicy(Qt::TabFocus);
		centralLayout->setWidget(2, QFormLayout::FieldRole, requestedDataLabel);
		centralWidget->setProperty("requestedData", requestedData);

		mUi->historyTableWidget->setCellWidget(rowIndex, 1, centralWidget);
	}

	mUi->historyTableWidget->resizeRowsToContents();
	mUi->historyTableWidget->sortByColumn(dateColumn, Qt::AscendingOrder); // Sort by date
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
				QWidget* tmpWidget = mUi->historyTableWidget->cellWidget(index.row(), dateColumn);
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

	auto& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
	settings.deleteSettings(deleteHistoryDialog->getTimePeriod());
	settings.save();
}


void HistoryWidget::exportHistory()
{
	QString filename = tr("AusweisApp2.History.%1.pdf").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd")));
	filename = QFileDialog::getSaveFileName(this,
			QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Save"),
			QDir::homePath() + QLatin1Char('/') + filename,
			tr("PDF Documents") + QStringLiteral(" (*.pdf)"));

	PdfExporter exporter(filename);
	exporter.exportHistory();
}


void HistoryWidget::searchHistory()
{
	mUi->historyTableWidget->setVisible(true);
	mUi->noResultWidget->setVisible(false);

	if (mUi->historySearch->text().isEmpty())
	{
		for (int i = 0; i < mUi->historyTableWidget->rowCount(); ++i)
		{
			mUi->historyTableWidget->setRowHidden(i, false);
		}
		return;
	}

	bool anyMatch = false;

	for (int i = 0; i < mUi->historyTableWidget->rowCount(); ++i)
	{
		bool match = false;
		for (int j = 0; j < mUi->historyTableWidget->columnCount(); ++j)
		{
			QString tmpData;
			QWidget* tmpWidget = mUi->historyTableWidget->cellWidget(i, j);
			if (j == dateColumn)
			{
				tmpData = tmpWidget->property("date").toString();
			}
			else
			{
				tmpData = tmpWidget->property("provider").toString() + QLatin1Char(' ') + tmpWidget->property("usage").toString() + QLatin1Char(' ') + tmpWidget->property("requestedData").toString();
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

	qDebug() << "selected model index:" << idx;

	if (pIndex.isValid())
	{
		QWidget* tmpWidget = mUi->historyTableWidget->cellWidget(idx.row(), dateColumn);
		DetailDialog d(this);
		d.setDetails(tmpWidget->property("termsOfUsage").toString());
		d.exec();
	}
}


void HistoryWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		init();
		searchHistory();
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
