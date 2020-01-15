/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "RandomPinDialog.h"
#include "ui_RandomPinDialog.h"

#include "HelpAction.h"
#include "Randomizer.h"
#include "ReaderManager.h"

#include <algorithm>
#include <QCloseEvent>

using namespace governikus;

static const char* PIN = "pin";

RandomPinDialog::RandomPinDialog(int pLength, const QString& pSelectedReader, QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::RandomPinDialog)
	, mSelectedReader(pSelectedReader)
{
	mUi->setupUi(this);
	mUi->pin->setMaxLength(pLength);

	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("On screen password"));

	setModal(true);

	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireCardRemoved, this, &RandomPinDialog::onCardRemoved);
	connect(readerManager, &ReaderManager::fireCardInserted, this, &RandomPinDialog::onCardInserted);

	installEventFilter(this);

	mUi->pinButtons->setId(mUi->button_pos_0, 0);
	mUi->pinButtons->setId(mUi->button_pos_1, 1);
	mUi->pinButtons->setId(mUi->button_pos_2, 2);
	mUi->pinButtons->setId(mUi->button_pos_3, 3);
	mUi->pinButtons->setId(mUi->button_pos_4, 4);
	mUi->pinButtons->setId(mUi->button_pos_5, 5);
	mUi->pinButtons->setId(mUi->button_pos_6, 6);
	mUi->pinButtons->setId(mUi->button_pos_7, 7);
	mUi->pinButtons->setId(mUi->button_pos_8, 8);
	mUi->pinButtons->setId(mUi->button_pos_9, 9);

	initComponents();
	createButton();
}


RandomPinDialog::~RandomPinDialog()
{
}


void RandomPinDialog::initComponents()
{
	mUi->clrButton->setIcon(QPixmap(QStringLiteral(":/images/randompin/btn_clear.png")));
	mUi->clrButton->setIconSize(QSize(44, 26));
	connect(mUi->clrButton, &QAbstractButton::clicked, mUi->pin, &QLineEdit::clear);

	mUi->cnlButton->setIcon(QPixmap(QStringLiteral(":/images/randompin/btn_cancel.png")));
	mUi->cnlButton->setIconSize(QSize(58, 50));
	connect(mUi->cnlButton, &QAbstractButton::clicked, this, &RandomPinDialog::reject);

	mUi->okButton->setIcon(QPixmap(QStringLiteral(":/images/randompin/btn_ok.png")));
	mUi->okButton->setIconSize(QSize(58, 50));
	connect(mUi->okButton, &QAbstractButton::clicked, this, &RandomPinDialog::accept);
}


void RandomPinDialog::createButton()
{
	QVector<int> buttonList = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	};
	std::shuffle(buttonList.begin(), buttonList.end(), Randomizer::getInstance().getGenerator());

	Q_ASSERT(buttonList.size() == mUi->pinButtons->buttons().size());
	for (int i = 0; i < buttonList.size(); ++i)
	{
		QAbstractButton* button = mUi->pinButtons->button(i);
		button->setIcon(QPixmap(QStringLiteral(":/images/randompin/btn_normal_%1.png").arg(buttonList.value(i))));
		button->setIconSize(QSize(58, 50));
		button->setProperty(PIN, QVariant::fromValue(buttonList.value(i)));
		connect(button, &QAbstractButton::clicked, this, &RandomPinDialog::onPosButtonClicked);
	}
}


QString RandomPinDialog::getPin()
{
	return mUi->pin->text();
}


void RandomPinDialog::onPosButtonClicked()
{
	auto* posButton = qobject_cast<QToolButton*>(sender());
	if (posButton)
	{
		mUi->pin->setText(mUi->pin->text() + posButton->property(PIN).toString());
	}
}


void RandomPinDialog::onCardRemoved(const QString& pReaderName)
{
	if (isVisible() && pReaderName == mSelectedReader)
	{
		reject();
	}
}


void RandomPinDialog::onCardInserted()
{
	if (isVisible())
	{
		reject();
	}
}


bool RandomPinDialog::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		auto* keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp();
			return true;
		}
	}
	return QDialog::eventFilter(pObject, pEvent);
}


void RandomPinDialog::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
