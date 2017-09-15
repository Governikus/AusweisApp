/*!
 * StepAuthenticationEac1Widget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StepAuthenticationEac1Widget.h"
#include "ui_StepAuthenticationEac1Widget.h"

#include "AppSettings.h"
#include "CardConnection.h"
#include "DetailDialog.h"
#include "generic/GuiUtils.h"
#include "generic/PasswordEdit.h"
#include "RandomPinDialog.h"

#include <QApplication>
#include <QCheckBox>
#include <QColor>
#include <QLineEdit>
#include <QLoggingCategory>
#include <QProgressBar>
#include <QtMath>
#include <QToolButton>
#include <QToolTip>
#include <QWindow>

#ifdef Q_OS_WIN32
#include <QWinTaskbarProgress>
#endif

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(gui)

StepAuthenticationEac1Widget::StepAuthenticationEac1Widget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::StepAuthenticationEac1Widget())
	, mContext()
	, mCANField(nullptr)
	, mPINField(nullptr)
	, mState(StepDidAuthenticateEac1Ui::State::INITIAL)
	, mProgressBar(nullptr)
	, mProgressBarLabel(nullptr)
	, mCloseWindowWhenFinished()
#ifdef Q_OS_WIN32
	, mTaskbarButton(new QWinTaskbarButton(this))
#endif
{
	mUi->setupUi(this);

	mUi->listWidgetWest->setAttribute(Qt::WA_MacShowFocusRect, false);
	mUi->listWidgetEast->setAttribute(Qt::WA_MacShowFocusRect, false);

	connect(mUi->detailsPushButton, &QPushButton::clicked, this, &StepAuthenticationEac1Widget::onDetailsButtonClicked);
}


StepAuthenticationEac1Widget::~StepAuthenticationEac1Widget()
{
}


void StepAuthenticationEac1Widget::setContext(const QSharedPointer<AuthContext>& pContext)
{
	mContext = pContext;
	#ifdef Q_OS_WIN32
	connect(mContext.data(), &AuthContext::fireResultChanged, this, &StepAuthenticationEac1Widget::onResultChanged);
	#endif
}


void StepAuthenticationEac1Widget::setState(StepDidAuthenticateEac1Ui::State pState)
{
	if (pState == mState)
	{
		return;
	}
	mState = pState;

	switch (pState)
	{
		case StepDidAuthenticateEac1Ui::State::INITIAL:
			break;

		case StepDidAuthenticateEac1Ui::State::EDIT_CHAT:
			setupChatView();
			return;

		case StepDidAuthenticateEac1Ui::State::ENTER_PIN:
			updateButtonsAndPinWidget();
			return;

		case StepDidAuthenticateEac1Ui::State::AUTHENTICATING_ESERVICE:
			Q_EMIT setCancelButtonState(ButtonState::ENABLED);
			updateProgressPanel(1, tr("Service provider is verified"));
			break;

		case StepDidAuthenticateEac1Ui::State::AUTHENTICATING_CARD:
			updateProgressPanel(2, tr("Card is being verified"));
			break;

		case StepDidAuthenticateEac1Ui::State::READING_CARD_DATA:
			updateProgressPanel(3, tr("Reading data"));
			break;

		case StepDidAuthenticateEac1Ui::State::REDIRECTING_BROWSER:
			updateProgressPanel(4, tr("Service provider is being verified"));
			break;

		case StepDidAuthenticateEac1Ui::State::FINISHED:
			updateProgressPanel();
			Q_EMIT setCancelButtonState(ButtonState::HIDDEN);
			Q_EMIT setForwardButtonState(ButtonState::FOCUSSED, tr("OK"));
			break;
	}
}


void StepAuthenticationEac1Widget::forwardStep()
{
	Q_EMIT setForwardButtonState(ButtonState::DISABLED, tr("Identify now"));

	if (mState == StepDidAuthenticateEac1Ui::State::EDIT_CHAT)
	{
		mUi->detailsPushButton->setEnabled(false);
		mUi->listWidgetWest->setEnabled(false);
		mUi->listWidgetEast->setEnabled(false);
	}
	else if (mState == StepDidAuthenticateEac1Ui::State::ENTER_PIN)
	{
		if (mContext->getCardConnection() != nullptr && mContext->getCardConnection()->getReaderInfo().isBasicReader())
		{
			mUi->pinGroupBox->setVisible(false);

			int childCount = mUi->pinWidgetLayout->count();
			for (int i = 0; i < childCount; ++i)
			{
				QLayoutItem* child = mUi->pinWidgetLayout->itemAt(i);
				if (child->widget() != nullptr)
				{
					child->widget()->setEnabled(false);
				}
			}
		}
		else
		{
			Q_EMIT setCancelButtonState(ButtonState::DISABLED);
		}
	}

}


void StepAuthenticationEac1Widget::updateButtonsAndPinWidget()
{
	Q_EMIT setCancelButtonState(ButtonState::ENABLED);

	if (mContext->getCardConnection()->getReaderInfo().getRetryCounter() == 1)
	{
		mUi->pinGroupBox->setTitle(tr("Please enter your six-digit card access number (CAN) and your PIN for identification."));
	}
	else
	{
		mUi->pinGroupBox->setTitle(tr("Please enter your six digit PIN for identification"));
	}

	if (mContext->getCardConnection()->getReaderInfo().isBasicReader())
	{
		clearPinWidgetLayout();
		createBasicReaderWidget();
		focusWidget();
	}
	else
	{
		clearPinWidgetLayout();
		QLabel* label = new QLabel(tr("Please pay attention to the display of your card reader."));
		label->setFocusPolicy(Qt::TabFocus);
		label->setObjectName(QStringLiteral("eac1PinInformationLabel"));
		mUi->pinWidgetLayout->invalidate();
		mUi->pinWidgetLayout->addWidget(label);
		Q_EMIT setCancelButtonState(ButtonState::DISABLED);
	}
	mUi->pinGroupBox->setVisible(true);
	Q_EMIT setForwardButtonState(ButtonState::DISABLED, tr("Identify now"));
}


void StepAuthenticationEac1Widget::clearPinWidgetLayout()
{
	while (QLayoutItem* child = mUi->pinWidgetLayout->itemAt(0))
	{
		if (QWidget* childWidget = child->widget())
		{
			mUi->pinWidgetLayout->removeWidget(childWidget);
			delete childWidget;
		}
		else
		{
			mUi->pinWidgetLayout->removeItem(child);
			delete child;
		}
	}

	mProgressBar = nullptr;
	mProgressBarLabel = nullptr;
}


void StepAuthenticationEac1Widget::onDetailsButtonClicked()
{
	DetailDialog d(this);

	auto eac1 = mContext->getDidAuthenticateEac1();
	CVCertificateBody body = eac1->getCvCertificates().at(0)->getBody();
	QString effectiveDate = body.getCertificateEffectiveDate().toString(Qt::DefaultLocaleShortDate);
	QString expirationDate = body.getCertificateExpirationDate().toString(Qt::DefaultLocaleShortDate);
	auto certificateDescription = eac1->getCertificateDescription();

	QString details;
	details += tr("Service provider:") + "\n";
	details += certificateDescription->getSubjectName();
	details += '\n';
	details += certificateDescription->getSubjectUrl();

	details += "\n\n" + tr("Certificate issuer:") + "\n";
	details += certificateDescription->getIssuerName();
	details += '\n';
	details += certificateDescription->getIssuerUrl();

	details += QLatin1String("\n\n");
	details += certificateDescription->getTermsOfUsage();
	details += QLatin1String("\n\n");
	details += tr("Validity:\n%1 - %2").arg(effectiveDate, expirationDate);

	// collapse multiple blank lines
	details.replace(QRegularExpression(QStringLiteral("\n\n\n*")), QStringLiteral("\n\n"));

	d.setDetails(details);
	d.exec();
}


void StepAuthenticationEac1Widget::setupChatView()
{
	auto eac1 = mContext->getDidAuthenticateEac1();
	mUi->subjectName->setText(eac1->getCertificateDescription()->getSubjectName());
	QString purpose = eac1->getCertificateDescription()->getPurpose();
	if (purpose.isEmpty())
	{
		purpose = tr("See details under more...");
	}
	mUi->usage->setText(purpose);

	if (eac1->getTransactionInfo().isNull() || eac1->getTransactionInfo().isEmpty())
	{
		mUi->transactionInfoGroupBox->setVisible(false);
	}
	else
	{
		mUi->transactionInfo->setText(eac1->getTransactionInfo());
		mUi->transactionInfoGroupBox->setVisible(true);
	}


	mUi->listWidgetWest->clear();
	mUi->listWidgetEast->clear();

	prepareChatsForGui();

	mUi->detailsPushButton->setEnabled(true);
	mUi->listWidgetWest->setEnabled(true);
	mUi->listWidgetEast->setEnabled(true);
	mUi->pinGroupBox->setVisible(false);

	Q_EMIT setCancelButtonState(ButtonState::ENABLED);
	Q_EMIT setForwardButtonState(ButtonState::FOCUSSED, tr("Identify now"));
}


void StepAuthenticationEac1Widget::prepareChatsForGui()
{
	const double optionalRightsCount = mContext->getOptionalAccessRights().size();
	const double requiredRightsCount = mContext->getRequiredAccessRights().size();
	int listSize = qCeil((optionalRightsCount + requiredRightsCount) / 2.0);

	for (AccessRight orderedRight : AccessRoleAndRightsUtil::allDisplayedOrderedRights())
	{
		if (mContext->getOptionalAccessRights().contains(orderedRight))
		{
			addChatRightToGui(orderedRight, true, listSize);
		}
		else if (mContext->getRequiredAccessRights().contains(orderedRight))
		{
			addChatRightToGui(orderedRight, false, listSize);
		}
	}
}


void StepAuthenticationEac1Widget::addChatRightToGui(AccessRight pRight, bool pOptional, int pListSize)
{
	QString displayText = AccessRoleAndRightsUtil::toDisplayText(pRight);
	if (pRight == AccessRight::AGE_VERIFICATION)
	{
		displayText += QStringLiteral(" (%1)").arg(mContext->getDidAuthenticateEac1()->getAuthenticatedAuxiliaryData()->getRequiredAge());
	}
	QCheckBox* cb = new QCheckBox(displayText);
	cb->setEnabled(pOptional);
	cb->setChecked(mContext->getEffectiveAccessRights().contains(pRight));

	mMap.insert(cb, pRight);

	connect(cb, &QCheckBox::stateChanged, this, &StepAuthenticationEac1Widget::checkBoxChanged);

	QListWidgetItem* item = new QListWidgetItem();
	item->setSizeHint(QSize(0, 20));
	if (mUi->listWidgetWest->count() < pListSize)
	{
		mUi->listWidgetWest->addItem(item);
		mUi->listWidgetWest->setItemWidget(item, cb);
	}
	else
	{
		mUi->listWidgetEast->addItem(item);
		mUi->listWidgetEast->setItemWidget(item, cb);
	}
}


void StepAuthenticationEac1Widget::createBasicReaderWidget()
{
	QWidget* basicReaderWidget = new QWidget();

	QHBoxLayout* basicReaderWidgetLayout = new QHBoxLayout(basicReaderWidget);

	AppSettings& appSettings = AppSettings::getInstance();

	QRegularExpression onlyNumbersExpression(QStringLiteral("[0-9]*"));
	if (mContext->getCardConnection()->getReaderInfo().getRetryCounter() == 1)
	{
		mCANField = new PasswordEdit();
		mCANField->setKeylessPasswordMode(false);
		mCANField->setFieldWidth(6);
		mCANField->setMaxLength(6);
		mCANField->configureValidation(onlyNumbersExpression, tr("Only digits (0-9) are permitted."));
		connect(mCANField, &PasswordEdit::textEdited, this, &StepAuthenticationEac1Widget::canTextEdited);

		QLabel* canLabel = new QLabel(tr("Card access number (CAN):"));
		canLabel->setFocusPolicy(Qt::TabFocus);
		basicReaderWidgetLayout->addWidget(canLabel);
		basicReaderWidgetLayout->addWidget(mCANField);

		if (appSettings.getGeneralSettings().isUseScreenKeyboard())
		{
			QToolButton* button = new QToolButton();
			button->setObjectName(QStringLiteral("canRandomButton"));
			button->setAccessibleName(tr("open on screen password dialog"));
			button->setAutoRaise(true);
			button->setIcon(QPixmap(QStringLiteral(":images/randompin/screen_keyboard.png")));
			button->setIconSize(QSize(44, 26));

			connect(button, &QAbstractButton::clicked, this, &StepAuthenticationEac1Widget::onRandomButtonClicked);

			basicReaderWidgetLayout->addWidget(button);
		}

	}

	mPINField = new PasswordEdit();
	mPINField->getLineEdit()->setAccessibleName(tr("More information with TAB"));
	mPINField->setKeylessPasswordMode(false);
	mPINField->setFieldWidth(6);
	mPINField->setMaxLength(6);
	mPINField->configureValidation(onlyNumbersExpression, tr("Only digits (0-9) are permitted."));
	connect(mPINField, &PasswordEdit::textEdited, this, &StepAuthenticationEac1Widget::pinTextEdited);

	if (mContext->getCardConnection()->getReaderInfo().getRetryCounter() == 1)
	{
		mPINField->setEnabled(false);
	}

	QLabel* pinLabel = new QLabel(tr("PIN:"));
	pinLabel->setFocusPolicy(Qt::TabFocus);
	basicReaderWidgetLayout->addWidget(pinLabel);
	basicReaderWidgetLayout->addWidget(mPINField);

	if (appSettings.getGeneralSettings().isUseScreenKeyboard())
	{
		QToolButton* button = new QToolButton();
		button->setObjectName(QStringLiteral("pinRandomButton"));
		button->setAccessibleName(tr("open on screen password dialog"));
		button->setAutoRaise(true);
		button->setIcon(QPixmap(QStringLiteral(":images/randompin/screen_keyboard.png")));
		button->setIconSize(QSize(44, 26));

		connect(button, &QAbstractButton::clicked, this, &StepAuthenticationEac1Widget::onRandomButtonClicked);

		basicReaderWidgetLayout->addWidget(button);
	}


	mUi->pinWidgetLayout->invalidate();
	mUi->pinWidgetLayout->addWidget(basicReaderWidget);
}


void StepAuthenticationEac1Widget::updateProgressPanel(int pProgressValue, const QString& pProgressText)
{
	if (pProgressValue > 0)
	{
		if (mProgressBar == nullptr)
		{
			clearPinWidgetLayout();
			QWidget* progressWidget = new QWidget();
			QVBoxLayout* progressWidgetLayout = new QVBoxLayout(progressWidget);
			progressWidgetLayout->setMargin(0);
			mUi->pinWidgetLayout->addWidget(progressWidget);

			mProgressBar = new QProgressBar();
			mProgressBar->setTextVisible(false);
			mProgressBar->setRange(0, 4);
			progressWidgetLayout->addWidget(mProgressBar);

			mProgressBarLabel = new QLabel();
			progressWidgetLayout->addWidget(mProgressBarLabel);

			mUi->pinGroupBox->setTitle(tr("Identify"));
			mUi->pinGroupBox->setVisible(true);
		}

		mProgressBar->setValue(pProgressValue);
		mProgressBarLabel->setText(pProgressText);
	}
	else
	{
		const bool cancelled = mContext->getStatus().isCancellationByUser();
		clearPinWidgetLayout();
		QWidget* doneWidget = new QWidget();
		QHBoxLayout* doneWidgetLayout = new QHBoxLayout(doneWidget);
		doneWidgetLayout->setMargin(0);
		mUi->pinWidgetLayout->addWidget(doneWidget);

		doneWidgetLayout->addStretch();
		QLabel* doneIcon = new QLabel;
		QLabel* doneText = new QLabel(cancelled ? tr("The process was cancelled by the user") : tr("Identification successful"));
		doneIcon->setPixmap(QPixmap(cancelled ? QStringLiteral(":/images/icon_cancelled.png") : QStringLiteral(":/images/icon_ok.png")));
		doneWidgetLayout->addWidget(doneIcon);
		doneWidgetLayout->addWidget(doneText);
		doneWidgetLayout->addStretch();

		mUi->pinGroupBox->setTitle(tr("Result"));
	}

#ifdef Q_OS_WIN32
	if (mTaskbarButton)
	{
		auto progress = mTaskbarButton->progress();
		progress->setValue(pProgressValue == 0 ? progress->maximum() : pProgressValue);
	}
#endif
}


void StepAuthenticationEac1Widget::checkBoxChanged(int pCheckState)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	if (cb != nullptr)
	{
		if (pCheckState == Qt::Unchecked)
		{
			QMap<QCheckBox*, AccessRight>::ConstIterator i = qAsConst(mMap).find(cb);
			qCDebug(gui) << "remove from effective chat: " << i.value();
			mContext->removeEffectiveAccessRight(i.value());
		}
		else
		{
			QMap<QCheckBox*, AccessRight>::ConstIterator i = qAsConst(mMap).find(cb);
			qCDebug(gui) << "set to effective chat: " << i.value();
			mContext->addEffectiveAccessRight(i.value());
		}
	}
}


void StepAuthenticationEac1Widget::onRandomButtonClicked()
{
	RandomPinDialog randomPinDialog(6, mContext->getReaderName(), this);
	if (randomPinDialog.exec() == QDialog::Accepted && !randomPinDialog.getPin().isEmpty())
	{
		QToolButton* pinButton = qobject_cast<QToolButton*>(sender());
		if (pinButton == nullptr)
		{
			qCCritical(gui) << "sender == nullptr";
		}
		else if (pinButton->objectName() == QLatin1String("canRandomButton"))
		{
			canTextEdited(randomPinDialog.getPin());
		}
		else if (pinButton->objectName() == QLatin1String("pinRandomButton"))
		{
			pinTextEdited(randomPinDialog.getPin());
		}
	}
}


void StepAuthenticationEac1Widget::onResultChanged()
{
#ifdef Q_OS_WIN32
	if (mTaskbarButton && (mContext.isNull() || mContext->getStatus().isError()))
	{
		mTaskbarButton->progress()->stop();
	}
#endif
}


void StepAuthenticationEac1Widget::hideEvent(QHideEvent* pEvent)
{
#ifdef Q_OS_WIN32
	if (mTaskbarButton)
	{
		mTaskbarButton->progress()->setVisible(false);
	}
#endif
	QWidget::hideEvent(pEvent);
}


void StepAuthenticationEac1Widget::showEvent(QShowEvent* pEvent)
{
#ifdef Q_OS_WIN32
	auto window = QApplication::activeWindow();
	if (window)
	{
		mTaskbarButton->setWindow(window->windowHandle());
		auto progress = mTaskbarButton->progress();
		progress->setVisible(true);
		progress->setRange(0, 5); // 5 == count of states in setState
		progress->reset();
		progress->resume(); // reset stop()
	}
#endif
	QWidget::showEvent(pEvent);
}


void StepAuthenticationEac1Widget::canTextEdited(const QString& pText)
{
	if (!pText.isNull() && !pText.isEmpty())
	{
		mCANField->getLineEdit()->setText(pText);
	}

	if (mCANField->text().size() == 6)
	{
		mPINField->setEnabled(true);
		if (mCANField->isLineEditActive())
		{
			mPINField->setFocus();
		}
		Q_EMIT fireCanUpdated(mCANField->text());
	}
	else
	{
		mPINField->setEnabled(false);
	}

	mPINField->clear();
	pinTextEdited(QString());
}


void StepAuthenticationEac1Widget::pinTextEdited(const QString& pText)
{
	if (!pText.isNull() && !pText.isEmpty())
	{
		mPINField->getLineEdit()->setText(pText);
	}

	if (mPINField->text().size() == 6)
	{
		if (mPINField->isLineEditActive())
		{
			Q_EMIT setForwardButtonState(ButtonState::FOCUSSED, tr("Identify now"));
		}
		else
		{
			Q_EMIT setForwardButtonState(ButtonState::ENABLED, tr("Identify now"));
		}
		Q_EMIT firePinUpdated(mPINField->text());
	}
	else
	{
		Q_EMIT setForwardButtonState(ButtonState::DISABLED, tr("Identify now"));
	}
}


void StepAuthenticationEac1Widget::focusWidget()
{
	if (mContext->getCardConnection()->getReaderInfo().getRetryCounter() == 1)
	{
		mCANField->setFocus();
		mCANField->setCursorPosition(0);
	}
	else
	{
		mPINField->setFocus();
		mPINField->setCursorPosition(0);
	}
}
