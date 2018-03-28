/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisWidget.h"

#include "LanguageLoader.h"
#include "ui_DiagnosisWidget.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextList>

// Includes for version API
#include <openssl/crypto.h>

#include "BuildHelper.h"
#include "context/DiagnosisContext.h"
#include "generic/HelpAction.h"

using namespace governikus;

class DiagnosisWidget::Field
{
	public:
		Field(QTextCursor& pCursor, const QString& pHeading, const QTextCharFormat& pPlainFormat, const QTextCharFormat& pHeadingFormat)
			: mStart()
			, mEnd()
		{
			pCursor.insertText(pHeading + QLatin1Char('\n'), pHeadingFormat);
			mStart = pCursor;
			mStart.setKeepPositionOnInsert(true);
			pCursor.insertText(tr("Diagnosis is running..."), pPlainFormat);
			mEnd = pCursor;
			mEnd.setKeepPositionOnInsert(true);
			pCursor.insertText(QStringLiteral("\n"), pPlainFormat);
		}


		void prepareSet()
		{
			mEnd.setKeepPositionOnInsert(false);
			mEnd.clearSelection();
			mEnd.setPosition(mStart.position(), QTextCursor::KeepAnchor);
		}


		void finishSet()
		{
			mEnd.setKeepPositionOnInsert(true);
		}


		const QTextCursor& getCursor() const
		{
			return mEnd;
		}


		void setText(const QString& pText, const QTextCharFormat& pPlainFormat)
		{
			prepareSet();
			mEnd.insertText(pText, pPlainFormat);
			finishSet();
		}


		void setFragment(const QTextDocumentFragment& pFragment)
		{
			prepareSet();
			mEnd.insertFragment(pFragment);
			finishSet();
		}


	private:
		QTextCursor mStart;
		QTextCursor mEnd;
};

DiagnosisWidget::DiagnosisWidget(DiagnosisContext* pContext, QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::DiagnosisWidget)
	, mContext(pContext)
	, mOsField()
	, mAppVersionField()
	, mPcscField()
	, mReadersField()
	, mTimestampField()
	, mPlainTextFormat()
	, mHeadingTextFormat()
	, mBasicBlockFormat()
	, mOsVersionTreeItem(nullptr)
	, mAppVersionTreeItem(nullptr)
	, mReadersTreeItem(nullptr)
	, mPcscTreeItem(nullptr)
	, mReaderWaitItem(nullptr)
	, mPcscWaitItem(nullptr)
	, mTimestampItem(nullptr)
{
	mUi->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
	setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Diagnosis"));

	QTextCursor insertCursor(mUi->infoTextEdit->document());
	mPlainTextFormat = insertCursor.charFormat();
	mHeadingTextFormat = mPlainTextFormat;
	mHeadingTextFormat.setFontWeight(QFont::Bold);
	mBasicBlockFormat = insertCursor.blockFormat();

	mOsField.reset(insertField(insertCursor, tr("Operating system"), true));
	mAppVersionField.reset(insertField(insertCursor, QCoreApplication::applicationName()));
	mReadersField.reset(insertField(insertCursor, tr("Card reader")));
	mPcscField.reset(insertField(insertCursor, tr("PC/SC")));
	mTimestampField.reset(insertField(insertCursor, tr("Time of diagnosis")));

	mOsVersionTreeItem = new QTreeWidgetItem(mUi->infoTreeWidget);
	mOsVersionTreeItem->setText(0, tr("Operating system"));
	mOsVersionTreeItem->setExpanded(true);

	(new QTreeWidgetItem(mOsVersionTreeItem))->setText(0, QSysInfo::prettyProductName());
	(new QTreeWidgetItem(mOsVersionTreeItem))->setText(0, QSysInfo::kernelVersion());
	(new QTreeWidgetItem(mOsVersionTreeItem))->setText(0, QSysInfo::currentCpuArchitecture());

	QStringList fields;
	fields << QSysInfo::prettyProductName();
	fields << QSysInfo::kernelVersion();
	fields << QSysInfo::currentCpuArchitecture();
	setFieldText(mOsField, fields.join(QLatin1Char('\n')));

	mPcscWaitItem = new QTreeWidgetItem(mPcscTreeItem);
	mPcscWaitItem->setText(0, tr("Diagnosis is running..."));

	mAppVersionTreeItem = new QTreeWidgetItem(mUi->infoTreeWidget);
	mAppVersionTreeItem->setText(0, QCoreApplication::applicationName());
	mAppVersionTreeItem->setExpanded(true);

	mReadersTreeItem = new QTreeWidgetItem(mUi->infoTreeWidget);
	mReadersTreeItem->setText(0, tr("Card reader"));
	mReadersTreeItem->setExpanded(true);
	mReaderWaitItem = new QTreeWidgetItem(mReadersTreeItem);
	mReaderWaitItem->setText(0, tr("Diagnosis is running..."));

	mPcscTreeItem = new QTreeWidgetItem(mUi->infoTreeWidget);
	mPcscTreeItem->setText(0, tr("PC/SC"));
	mPcscTreeItem->setExpanded(true);
	mPcscWaitItem = new QTreeWidgetItem(mPcscTreeItem);
	mPcscWaitItem->setText(0, tr("Diagnosis is running..."));

	mTimestampItem = new QTreeWidgetItem(mUi->infoTreeWidget);
	mTimestampItem->setExpanded(true);
	mTimestampItem->setText(0, tr("Time of diagnosis"));

	const QStringList appVersion({
				QStringLiteral("%1 (%2)").arg(QCoreApplication::applicationVersion(), QString::fromLatin1(BuildHelper::getDateTime())),
				QCoreApplication::organizationName(),
				QStringLiteral("Qt ") + QString::fromLatin1(qVersion()),
				QString::fromLatin1(SSLeay_version(0))
			});
	for (const auto& str : appVersion)
	{
		(new QTreeWidgetItem(mAppVersionTreeItem))->setText(0, str);
	}
	setFieldText(mAppVersionField, appVersion.join(QLatin1Char('\n')));

	connect(mContext, &DiagnosisContext::pcscInfoChanged, this, &DiagnosisWidget::onPcscInfoChanged);
	connect(mContext, &DiagnosisContext::readerInfosChanged, this, &DiagnosisWidget::onReaderInfosChanged);
	connect(mContext, &DiagnosisContext::timestampChanged, this, &DiagnosisWidget::onTimestampChanged);

	mUi->infoTextEdit->setVisible(false);
}


DiagnosisWidget::~DiagnosisWidget()
{
}


bool DiagnosisWidget::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp();
			return true;
		}
	}
	return QWidget::eventFilter(pObject, pEvent);
}


void DiagnosisWidget::onPcscInfoChanged()
{
	mPcscTreeItem->removeChild(mPcscWaitItem);
	delete mPcscWaitItem;
	mPcscWaitItem = nullptr;

	mPcscField->prepareSet();

	QTextCursor insertCursor = mPcscField->getCursor();
	insertCursor.removeSelectedText();

	//version
	insertCursor.insertText(tr("Version: %1").arg(mContext->getPcscVersion()), mPlainTextFormat);
	endBlockAndResetFormat(insertCursor);
	(new QTreeWidgetItem(mPcscTreeItem))->setText(0, tr("Version: %1").arg(mContext->getPcscVersion()));

	//components
	QTreeWidgetItem* componentTreeItem = new QTreeWidgetItem(mPcscTreeItem);
	componentTreeItem->setText(0, tr("Components"));
	insertComponentList(insertCursor, tr("Components:"), mContext->getPcscComponents(), componentTreeItem);

	//driver
	QTreeWidgetItem* driverTreeItem = new QTreeWidgetItem(mPcscTreeItem);
	driverTreeItem->setText(0, tr("Driver"));
	insertComponentList(insertCursor, tr("Driver:"), mContext->getPcscDrivers(), driverTreeItem);

	mPcscField->finishSet();

}


void DiagnosisWidget::onReaderInfosChanged()
{
	mReadersTreeItem->removeChild(mReaderWaitItem);
	delete mReaderWaitItem;
	mReaderWaitItem = nullptr;

	const QVector<ReaderInfo>& infos = mContext->getReaderInfos();
	if (infos.isEmpty())
	{
		setFieldText(mReadersField, tr("not recognised"));
		(new QTreeWidgetItem(mReadersTreeItem))->setText(0, tr("not recognised"));
		return;
	}

	mReadersField->prepareSet();

	QTextCursor insertCursor = mReadersField->getCursor();
	insertCursor.removeSelectedText();
	QTextListFormat listFormat;
	listFormat.setStyle(QTextListFormat::ListDisc);
	QTextList* list = insertCursor.insertList(listFormat);

	bool isFirst = true;
	for (const ReaderInfo& info : infos)
	{
		if (isFirst)
		{
			isFirst = false;
		}
		else
		{
			insertCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
		}

		// Reset the indent. Otherwise each list element would be indented further.
		QTextBlockFormat blockFormat = insertCursor.blockFormat();
		blockFormat.setIndent(0);
		insertCursor.setBlockFormat(blockFormat);

		// reader name
		insertCursor.insertText(info.getName(), mPlainTextFormat);
		list->add(insertCursor.block());
		QTreeWidgetItem* readerTreeItem = new QTreeWidgetItem(mReadersTreeItem);
		readerTreeItem->setText(0, info.getName());

		insertCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
		list->remove(insertCursor.block());

		// reader type
		QString readerType = info.isBasicReader() ? tr("Basic card reader") : tr("Standard / deluxe card reader");
		insertCursor.insertText(tr("Type: %1").arg(readerType), mPlainTextFormat);
		(new QTreeWidgetItem(readerTreeItem))->setText(0, tr("Type: %1").arg(readerType));

		// card type
		QString cardType = info.getCardTypeString();

		insertCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
		insertCursor.insertText(tr("Card: %1").arg(cardType), mPlainTextFormat);
		(new QTreeWidgetItem(readerTreeItem))->setText(0, tr("Card: %1").arg(cardType));


		// retry counter
		if (info.hasEidCard())
		{
			insertCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
			insertCursor.insertText(tr("Retry counter: %1").arg(3 - info.getRetryCounter()), mPlainTextFormat);
			(new QTreeWidgetItem(readerTreeItem))->setText(0, tr("Retry counter: %1").arg(3 - info.getRetryCounter()));
		}
	}

	mReadersField->finishSet();
}


void DiagnosisWidget::onTimestampChanged()
{
	QString timestamp = LanguageLoader::getInstance().getUsedLocale().toString(getCreationTime(), tr("d. MMMM yyyy, hh:mm:ss AP"));
	setFieldText(mTimestampField, timestamp);
	(new QTreeWidgetItem(mTimestampItem))->setText(0, timestamp);

}


QString DiagnosisWidget::getInfoTextEdit() const
{
	return mUi->infoTextEdit->toPlainText();
}


QDateTime DiagnosisWidget::getCreationTime() const
{
	return mContext->getTimestamp();
}


DiagnosisWidget::Field* DiagnosisWidget::insertField(QTextCursor& pCursor, const QString& pHeading, bool pIsFirstField)
{
	if (!pIsFirstField)
	{
		pCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
	}

	return new Field(pCursor, pHeading, mPlainTextFormat, mHeadingTextFormat);
}


void DiagnosisWidget::setFieldText(const QScopedPointer<Field>& pField, const QString& pText)
{
	pField->setText(pText, mPlainTextFormat);
}


void DiagnosisWidget::endBlockAndResetFormat(QTextCursor& pCursor)
{
	pCursor.insertText(QStringLiteral("\n"));
	pCursor.setBlockFormat(mBasicBlockFormat);
}


void DiagnosisWidget::insertComponentList(QTextCursor& pCursor, const QString& pTitle,
		const QVector<DiagnosisContext::ComponentInfo>& pComponents, QTreeWidgetItem* pParentTreeWidgetItem)
{
	if (pComponents.isEmpty())
	{
		return;
	}

	pCursor.insertText(pTitle, mPlainTextFormat);

	QTextListFormat listFormat;
	listFormat.setStyle(QTextListFormat::ListDisc);
	QTextList* list = pCursor.insertList(listFormat);

	bool isFirst = true;
	for (const DiagnosisContext::ComponentInfo& info : pComponents)
	{
		if (isFirst)
		{
			isFirst = false;
		}
		else
		{
			pCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
		}

		// Reset the indent. Otherwise each list element would be indented further.
		QTextBlockFormat blockFormat = pCursor.blockFormat();
		blockFormat.setIndent(0);
		pCursor.setBlockFormat(blockFormat);

		// description
		pCursor.insertText(info.getDescription(), mPlainTextFormat);
		QTreeWidgetItem* descriptionTreeItem = new QTreeWidgetItem(pParentTreeWidgetItem);
		descriptionTreeItem->setText(0, info.getDescription());

		list->add(pCursor.block());

		pCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
		list->remove(pCursor.block());

		// company
		pCursor.insertText(tr("Vendor: %1").arg(info.getManufacturer()), mPlainTextFormat);
		(new QTreeWidgetItem(descriptionTreeItem))->setText(0, tr("Vendor: %1").arg(info.getManufacturer()));

		// version
		pCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
		pCursor.insertText(tr("Version: %1").arg(info.getVersion()), mPlainTextFormat);
		(new QTreeWidgetItem(descriptionTreeItem))->setText(0, tr("Version: %1").arg(info.getVersion()));

		// path
		pCursor.insertText(QStringLiteral("\n"), mPlainTextFormat);
		pCursor.insertText(tr("File path: %1").arg(info.getPath()), mPlainTextFormat);
		(new QTreeWidgetItem(descriptionTreeItem))->setText(0, tr("File path: %1").arg(info.getPath()));
	}

	endBlockAndResetFormat(pCursor);
}


void DiagnosisWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
