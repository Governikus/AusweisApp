/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDeviceWidget.h"
#include "ui_ReaderDeviceWidget.h"

#include "ReaderConfiguration.h"
#include "RemoteClient.h"
#include "RemotePinInputDialog.h"

#include <QLoggingCategory>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


ReaderDeviceWidget::ReaderDeviceWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::ReaderDeviceWidget)
	, mLocalReaderDataModel(this)
	, mRemoteReaderDataModel(this)
{
	mUi->setupUi(this);

	setDisplayText();

	mUi->tableViewLocal->setModel(&mLocalReaderDataModel);
	mUi->tableViewLocal->horizontalHeader()->setStretchLastSection(true);
	mUi->tableViewLocal->verticalHeader()->setVisible(false);

	mUi->tableViewRemote->setModel(&mRemoteReaderDataModel);
	mUi->tableViewRemote->horizontalHeader()->setStretchLastSection(true);
	mUi->tableViewRemote->verticalHeader()->setVisible(false);

	mUi->infoText->setOpenExternalLinks(true);

	connect(Env::getSingleton<ReaderConfiguration>(), &ReaderConfiguration::fireUpdated, this, &ReaderDeviceWidget::onAdjustReaderNameColumnWidth);
	onAdjustReaderNameColumnWidth();

	connect(&mLocalReaderDataModel, &ReaderDriverModel::fireModelChanged, this, &ReaderDeviceWidget::onUpdateLocalTableSelection);
	connect(&mRemoteReaderDataModel, &RemoteDeviceModel::fireModelChanged, this, &ReaderDeviceWidget::onUpdateRemoteTableSelection);
	onUpdateLocalTableSelection();
	onUpdateRemoteTableSelection();

	connect(mUi->tableViewRemote->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ReaderDeviceWidget::onRemoteSelectionChanged);
	connect(mUi->tableViewLocal->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ReaderDeviceWidget::onUpdateInfo);
	onRemoteSelectionChanged();
	onUpdateInfo();

	connect(mUi->connectRemote, &QPushButton::clicked, this, &ReaderDeviceWidget::onConnectClicked);
	connect(mUi->forgetRemote, &QPushButton::clicked, this, &ReaderDeviceWidget::onForgetClicked);
	connect(mUi->tableViewRemote, &QTableView::doubleClicked, this, &ReaderDeviceWidget::onRemoteDoubleClicked);

	auto* const remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireDispatcherDestroyed, &mRemoteReaderDataModel, &RemoteDeviceModel::onDeviceDisconnected);
}


ReaderDeviceWidget::~ReaderDeviceWidget()
{
}


void ReaderDeviceWidget::prependAccessibleName(const QString& pAccessibleNameAddition)
{
	mUi->cardReaderDescription->setAccessibleName(pAccessibleNameAddition + mUi->cardReaderDescription->accessibleName());
}


void ReaderDeviceWidget::onRemoteSelectionChanged()
{
	const QItemSelectionModel* const selectionModel = mUi->tableViewRemote->selectionModel();
	const QModelIndexList& selectionList = selectionModel->selectedRows();

	if (selectionList.isEmpty())
	{
		mUi->connectRemote->setEnabled(false);
		mUi->forgetRemote->setEnabled(false);
	}
	else
	{
		const QModelIndex& index = selectionList.at(0);
		if (mRemoteReaderDataModel.isPaired(index))
		{
			mUi->connectRemote->setEnabled(false);
			mUi->forgetRemote->setEnabled(true);
		}
		else
		{
			mUi->connectRemote->setEnabled(mRemoteReaderDataModel.isSupported(index));
			mUi->forgetRemote->setEnabled(false);
		}
	}
}


void ReaderDeviceWidget::onUpdateInfo()
{
	updateInfoIcon();
	updateInfoText();
	updateInfoUpdate();
}


void ReaderDeviceWidget::setDisplayText()
{
	const QString& remoteEmptyListDescriptionString = mRemoteReaderDataModel.getEmptyListDescriptionString();
	mUi->remoteEmptyListDescription->setText(remoteEmptyListDescriptionString);
	mUi->remoteEmptyListDescription->setAccessibleName(remoteEmptyListDescriptionString);

	const QString& localEmptyListDescriptionString = mLocalReaderDataModel.getEmptyListDescriptionString();
	mUi->localEmptyListDescription->setText(localEmptyListDescriptionString);
	mUi->localEmptyListDescription->setAccessibleDescription(localEmptyListDescriptionString);
}


void ReaderDeviceWidget::updateInfoIcon()
{
	const QItemSelectionModel* const selectionModel = mUi->tableViewLocal->selectionModel();
	const QModelIndexList& selectionList = selectionModel->selectedRows();

	QPixmap pixmap;
	if (selectionList.isEmpty())
	{
		pixmap = QPixmap(mLocalReaderDataModel.getNoReaderFoundIconPath());
	}
	else
	{
		const QModelIndex& index = selectionList.at(0);
		const QString& path = mLocalReaderDataModel.getReaderImagePath(index);
		pixmap = QPixmap(path);
		if (mLocalReaderDataModel.isInstalledSupportedReader(index))
		{
			QPixmap checkMark(QStringLiteral(":/images/status_ok.svg"));
			checkMark = checkMark.scaledToHeight(pixmap.height() / 3, Qt::SmoothTransformation);

			const int insertAtX = pixmap.width() - checkMark.width();
			const int insertAtY = pixmap.height() / 5 - checkMark.height() / 2;
			QPainter(&pixmap).drawPixmap(insertAtX, insertAtY, checkMark);
		}
	}
	const int layoutHeight = mUi->detailInfoLayout->geometry().height();
	mUi->readerLabel->setPixmap(pixmap.scaledToHeight(layoutHeight / 2, Qt::SmoothTransformation));
}


void ReaderDeviceWidget::updateInfoText()
{
	const QItemSelectionModel* const selectionModel = mUi->tableViewLocal->selectionModel();
	const QModelIndexList& selectionList = selectionModel->selectedRows();

	QString infoText;
	if (selectionList.isEmpty())
	{
		if (mLocalReaderDataModel.rowCount() == 0)
		{
			infoText = tr("Please connect a suitable card reader.");
		}
		else
		{
			infoText = tr("Select a device to display more information about it");
		}
	}
	else
	{
		const QModelIndex& index = selectionList.at(0);
		infoText = mLocalReaderDataModel.getHTMLDescription(index);
	}

	if (infoText.isEmpty())
	{
		mUi->stackedWidget->setCurrentWidget(mUi->emptyWidget);
	}
	else
	{
		mUi->infoText->setHtml(QStringLiteral("<html><body><p>") + infoText + QStringLiteral("</p></body></html>"));
		mUi->stackedWidget->setCurrentWidget(mUi->infoText);
	}
}


void ReaderDeviceWidget::updateInfoUpdate()
{
	mUi->updateTimeLabel->setText(mLocalReaderDataModel.getLastUpdatedInformation());
}


void ReaderDeviceWidget::onUpdateLocalTableSelection()
{
	if (mLocalReaderDataModel.rowCount() > 0)
	{
		mUi->localEmptyListDescriptionFrame->setVisible(false);
		mUi->tableViewLocal->setVisible(true);
		QItemSelectionModel* const selectionModel = mUi->tableViewLocal->selectionModel();
		if (selectionModel->selectedRows().isEmpty())
		{
			selectionModel->select(mLocalReaderDataModel.index(0, ReaderDriverModel::ColumnId::ReaderName), QItemSelectionModel::Select);
			selectionModel->select(mLocalReaderDataModel.index(0, ReaderDriverModel::ColumnId::ReaderStatus), QItemSelectionModel::Select);
		}
	}
	else
	{
		mUi->tableViewLocal->setVisible(false);
		mUi->localEmptyListDescriptionFrame->setVisible(true);
	}
	onUpdateInfo();
}


void ReaderDeviceWidget::onUpdateRemoteTableSelection()
{
	if (mRemoteReaderDataModel.rowCount() > 0)
	{
		mUi->remoteEmptyListDescriptionFrame->setVisible(false);
		mUi->tableViewRemote->setVisible(true);
		QItemSelectionModel* const selectionModel = mUi->tableViewRemote->selectionModel();
		if (selectionModel->selectedRows().isEmpty())
		{
			selectionModel->select(mRemoteReaderDataModel.index(0, RemoteDeviceModel::ColumnId::ReaderName), QItemSelectionModel::Select);
			selectionModel->select(mRemoteReaderDataModel.index(0, RemoteDeviceModel::ColumnId::ReaderStatus), QItemSelectionModel::Select);
		}
	}
	else
	{
		mUi->tableViewRemote->setVisible(false);
		mUi->remoteEmptyListDescriptionFrame->setVisible(true);
		onRemoteSelectionChanged();
	}
}


void ReaderDeviceWidget::onAdjustReaderNameColumnWidth()
{
	const auto& infos = Env::getSingleton<ReaderConfiguration>()->getReaderConfigurationInfos();

	int maxWidth = 0;
	const QFontMetrics metrics(QGuiApplication::font());
	for (const auto& info : infos)
	{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
		const int deviceNameWidth = metrics.horizontalAdvance(info.getName());
#else
		const int deviceNameWidth = metrics.width(info.getName());
#endif
		if (deviceNameWidth > maxWidth)
		{
			maxWidth = deviceNameWidth;
		}
	}

	if (maxWidth <= 0)
	{
		return;
	}

	const int CELL_PADDING = 20;
	mUi->tableViewLocal->setColumnWidth(0, maxWidth + CELL_PADDING);
	mUi->tableViewRemote->setColumnWidth(0, maxWidth + CELL_PADDING);
}


void ReaderDeviceWidget::onConnectClicked()
{
	const auto& selectionModel = mUi->tableViewRemote->selectionModel();
	const QModelIndexList& selectionList = selectionModel->selectedRows();

	if (!selectionList.isEmpty())
	{
		const QModelIndex& index = selectionList.at(0);

		const QSharedPointer<RemoteDeviceListEntry> remoteDeviceListEntry = mRemoteReaderDataModel.getRemoteDeviceListEntry(index);
		if (remoteDeviceListEntry.isNull())
		{
			return;
		}

		setEnabled(false);

		QMessageBox pairingInfoBox(this);
		pairingInfoBox.setText(tr("Please start pairing mode first."));
		pairingInfoBox.setWindowModality(Qt::WindowModal);
		pairingInfoBox.setWindowFlags(pairingInfoBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
		pairingInfoBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		pairingInfoBox.setDefaultButton(QMessageBox::Ok);
		pairingInfoBox.button(QMessageBox::Ok)->setFocus();
		pairingInfoBox.setIconPixmap(QIcon(QStringLiteral(":/images/npa.svg")).pixmap(32, 32));

		if (pairingInfoBox.exec() == QMessageBox::Cancel)
		{
			setEnabled(true);
			return;
		}

		const QString pin = RemotePinInputDialog::getPin(this);
		if (!pin.isEmpty())
		{
			auto* const remoteClient = Env::getSingleton<RemoteClient>();
			connect(remoteClient, &RemoteClient::fireEstablishConnectionDone, this, &ReaderDeviceWidget::onEstablishConnectionDone);
			remoteClient->establishConnection(remoteDeviceListEntry, pin);
		}

		setEnabled(true);
	}
}


void ReaderDeviceWidget::onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus)
{
	Q_UNUSED(pEntry)
	auto* const remoteClient = Env::getSingleton<RemoteClient>();
	disconnect(remoteClient, &RemoteClient::fireEstablishConnectionDone, this, &ReaderDeviceWidget::onEstablishConnectionDone);
	if (pStatus.isError())
	{
		QMessageBox box(QApplication::activeWindow());
		box.setIcon(QMessageBox::Critical);
		box.setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("Pairing"));
		box.setWindowIcon(QIcon(QStringLiteral(":/images/npa.svg")));
		box.setWindowModality(Qt::WindowModal);
		box.setText(pStatus.toErrorDescription());
		box.setStandardButtons(QMessageBox::Ok);
		box.button(QMessageBox::Ok)->setFocus();
		box.exec();
	}
}


void ReaderDeviceWidget::onForgetClicked()
{
	const auto& selectionModel = mUi->tableViewRemote->selectionModel();
	const QModelIndexList& selectionList = selectionModel->selectedRows();

	if (!selectionList.isEmpty())
	{
		const QModelIndex& index = selectionList.at(0);

		mRemoteReaderDataModel.forgetDevice(index);
	}
}


void ReaderDeviceWidget::showEvent(QShowEvent* pEevent)
{
	onUpdateInfo();
	Q_EMIT fireWidgetShown();
	QWidget::showEvent(pEevent);
}


void ReaderDeviceWidget::hideEvent(QHideEvent* pEvent)
{
	Q_EMIT fireWidgetHidden();
	QWidget::hideEvent(pEvent);
}


void ReaderDeviceWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
		setDisplayText();
		onUpdateInfo();
	}
	QWidget::changeEvent(pEvent);
}


void ReaderDeviceWidget::onRemoteDoubleClicked(const QModelIndex& pIndex)
{
	if (!mRemoteReaderDataModel.isPaired(pIndex) && mRemoteReaderDataModel.isSupported((pIndex)))
	{
		onConnectClicked();
	}
}
