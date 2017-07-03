#include "ReaderDriverWidget.h"

#include "DriverSettings.h"
#include "ReaderManager.h"
#include "generic/SmartCardUtil.h"
#include "ui_ReaderDriverWidget.h"

#include <QLoggingCategory>
#include <QPixmap>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


static const int DEVICE_NAME_COLUMN = 0;
static const int DEVICE_STATUS_COLUMN = 1;
static const int DEVICE_IMAGE_COLUMN = 2;

static QTableWidgetItem* makeReadOnlyItem(const QString& text)
{
	QTableWidgetItem* const item = new QTableWidgetItem(text);
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);

	return item;
}


ReaderDriverWidget::ReaderDriverWidget(QWidget* pParent)
	: Page(tr("Reader driver integration"), pParent)
	, ui(new Ui::ReaderDriverWidget)
	, mSelectedRow(-1)
{
	QStringList headerLabels({QString(), QString()});
	headerLabels[DEVICE_NAME_COLUMN] = tr("Device");
	headerLabels[DEVICE_STATUS_COLUMN] = tr("Status");

	ui->setupUi(this);
	ui->labelLayout->setAlignment(ui->readerLabel, Qt::AlignCenter);
	ui->tableWidget->setColumnCount(3);
	ui->tableWidget->setColumnWidth(DEVICE_NAME_COLUMN, getLongestReaderNameWidth());
	ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui->tableWidget->verticalHeader()->setVisible(false);
	ui->tableWidget->setColumnHidden(2, true);

	ui->infoText->setOpenExternalLinks(true);

	mTimer.setSingleShot(true);
	mTimer.setInterval(10000);
	connect(&mTimer, &QTimer::timeout, this, &ReaderDriverWidget::onTimerEvent);

	connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, &ReaderDriverWidget::onDeviceSelectionChanged);

	connect(&mContext, &ReaderDriverContext::fireReaderChangeDetected, this, &ReaderDriverWidget::onUpdateContent);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &ReaderDriverWidget::onUpdateContent);
}


ReaderDriverWidget::~ReaderDriverWidget()
{
	delete ui;
}


void ReaderDriverWidget::onUpdateContent()
{
	mAttachedDevices = mContext.attachedDevices();
	qCDebug(gui) << "Found" << mAttachedDevices.size() << "attached devices";

	removeOldTableItems();
	updateTableItems();
	if (ui->tableWidget->rowCount() > 0 && ui->tableWidget->currentRow() == -1)
	{
		ui->tableWidget->selectRow(0);
	}
	onDeviceSelectionChanged();

	mTimer.start();
}


void ReaderDriverWidget::onDeviceSelectionChanged()
{
	mSelectedRow = ui->tableWidget->currentRow();
	updateInfo();
}


void ReaderDriverWidget::removeOldTableItems()
{
	for (int i = ui->tableWidget->rowCount() - 1; i >= 0; i--)
	{
		QString deviceName = ui->tableWidget->item(i, DEVICE_NAME_COLUMN)->text();
		bool found = false;

		for (const auto& dev : mAttachedDevices)
		{
			if (dev->getName() == deviceName)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			ui->tableWidget->removeRow(i);
		}
	}
}


void ReaderDriverWidget::updateTableItems()
{
	for (const auto& dev : mAttachedDevices)
	{
		const QString statusText = dev->hasDriver() ? tr("Installed") : tr("Searching for driver...");
		const QString deviceName = dev->getName();
		bool found = false;

		for (int i = 0; i < ui->tableWidget->rowCount(); i++)
		{
			if (ui->tableWidget->item(i, DEVICE_NAME_COLUMN)->text() == deviceName)
			{
				if (dev->hasDriver())
				{
					ui->tableWidget->setItem(i, DEVICE_STATUS_COLUMN, makeReadOnlyItem(statusText));
				}
				found = true;
				break;
			}
		}

		if (!found)
		{
			const int lastRow = ui->tableWidget->rowCount();
			ui->tableWidget->insertRow(lastRow);
			ui->tableWidget->setItem(lastRow, DEVICE_NAME_COLUMN, makeReadOnlyItem(dev->getName()));
			ui->tableWidget->setItem(lastRow, DEVICE_STATUS_COLUMN, makeReadOnlyItem(statusText));
			const QString path = SmartCardUtil::getReaderEmptyIconPath(dev->getReaderType());
			ui->tableWidget->setItem(lastRow, DEVICE_IMAGE_COLUMN, makeReadOnlyItem(path));
		}
	}
}


void ReaderDriverWidget::updateInfo()
{
	updateInfoIcon();
	updateInfoText();
	updateInfoUpdate();
}


void ReaderDriverWidget::updateInfoIcon()
{
	QString path;
	if (mSelectedRow == -1)
	{
		path = SmartCardUtil::getNoReaderFoundIconPath();
	}
	else
	{
		path = ui->tableWidget->item(mSelectedRow, DEVICE_IMAGE_COLUMN)->text();
	}
	QPixmap pixmap(path);
	const int layoutHeight = ui->detailInfoLayout->geometry().height();
	ui->readerLabel->setPixmap(pixmap.scaledToHeight(layoutHeight / 2, Qt::SmoothTransformation));
}


void ReaderDriverWidget::updateInfoText()
{
	if (mSelectedRow == -1)
	{
		if (ui->tableWidget->rowCount() == 0)
		{
			ui->infoText->setHtml(tr("<html><body><p>No card reader detected</p></body></html>"));
		}
		else
		{
			ui->infoText->setHtml(tr("<html><body><p>Select a device to display more information about it</p></body></html>"));
		}
	}
	else
	{
		if (mSelectedRow < 0 || mSelectedRow >= mAttachedDevices.size() || mSelectedRow >= ui->tableWidget->rowCount())
		{
			// We should never get in here
			qCWarning(gui) << "mSelectedRow out of range, skipping update";
			qCWarning(gui) << "    mSelectedRow                =" << mSelectedRow;
			qCWarning(gui) << "    mAttachedDevices.size()     =" << mAttachedDevices.size();
			qCWarning(gui) << "    ui->tableWidget->rowCount() =" << ui->tableWidget->rowCount();
		}
		else if (ui->tableWidget->item(mSelectedRow, DEVICE_STATUS_COLUMN)->text() == tr("Installed"))
		{
			ui->infoText->setHtml(tr("<html><body><p>Device is installed correctly</p></body></html>"));
		}
		else
		{
			const QSharedPointer<DeviceDescriptor> selectedDevice = mAttachedDevices.at(mSelectedRow);
			const QString newText = tr("<html><body><p>Device is not configured, please download and install the driver "
									   "you can find at url: <a href=\"%1\">%1</a></p></body></html>").
					arg(selectedDevice->getDriverUrl());
			ui->infoText->setHtml(newText);
		}
	}
}


void ReaderDriverWidget::updateInfoUpdate()
{
	QTime now = QTime::currentTime();
	const QString text = tr("The list of card readers was last updated at %1.");

	ui->updateTimeLabel->setText(text.arg(now.toString(tr("hh:mm:ss AP"))));
}


int ReaderDriverWidget::getLongestReaderNameWidth() const
{
	static const int CELL_PADDING = 20;

	QSharedPointer<DriverSettings> settings(new DriverSettings());
	settings->load();

	int maxWidth = 0;
	const QFontMetrics metrics(QGuiApplication::font());
	for (const auto& driver : settings->getDrivers())
	{
		const int deviceNameWidth = metrics.width(driver->getName());
		if (deviceNameWidth > maxWidth)
		{
			maxWidth = deviceNameWidth;
		}
	}

	return maxWidth + CELL_PADDING;
}


void ReaderDriverWidget::onTimerEvent()
{
	for (int i = 0; i < ui->tableWidget->rowCount(); i++)
	{
		if (ui->tableWidget->item(i, DEVICE_STATUS_COLUMN)->text() == tr("Searching for driver..."))
		{
			ui->tableWidget->setItem(i, DEVICE_STATUS_COLUMN, makeReadOnlyItem(tr("Not installed")));
		}
	}
}
