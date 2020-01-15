/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowQtWidget.h"

#include <algorithm>
#include <QLabel>

using namespace governikus;

WorkflowQtWidget::WorkflowQtWidget(QWidget* pParent)
	: QWidget(pParent)
	, mMainLayout(new QVBoxLayout(this))
	, mStepWidgetArea(new QWidget(this))
	, mCancelButton(nullptr)
	, mForwardButton(nullptr)
{
	(new QVBoxLayout(mStepWidgetArea))->setMargin(0);
	mMainLayout->addWidget(mStepWidgetArea);
	mMainLayout->setMargin(0);

	auto* buttonLayout = new QHBoxLayout();
	buttonLayout->setMargin(0);
	mMainLayout->addLayout(buttonLayout);

	auto* horizontalSpacer = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Ignored);

	buttonLayout->addItem(horizontalSpacer);

	mCancelButton = new QPushButton(this);
	connect(mCancelButton, &QPushButton::clicked, this, &WorkflowQtWidget::onCancelButtonClicked);
	mCancelButton->setVisible(false);
	buttonLayout->addWidget(mCancelButton);
	setCancelButtonState(ButtonState::HIDDEN);

	mForwardButton = new QPushButton(this);
	connect(mForwardButton, &QPushButton::clicked, this, &WorkflowQtWidget::forwardStep);
	mForwardButton->setVisible(false);
	buttonLayout->addWidget(mForwardButton);
	setForwardButtonState(ButtonState::HIDDEN);
}


WorkflowQtWidget::~WorkflowQtWidget()
{
}


void WorkflowQtWidget::addStepWidget(QWidget* pStepWidget)
{
	mStepWidgetArea->layout()->addWidget(pStepWidget);
}


void WorkflowQtWidget::removeStepWidget(QWidget* pStepWidget)
{
	mStepWidgetArea->layout()->removeWidget(pStepWidget);
	pStepWidget->setParent(nullptr); // Remove widget from display
	// Note: The Step* widgets are deleted in the Step* dtors.
}


void WorkflowQtWidget::setForwardButtonState(ButtonState pState, const QString& pText)
{
	setButtonState(mForwardButton, pState, pText.isEmpty() ? tr("Next") : pText);
}


void WorkflowQtWidget::setCancelButtonState(ButtonState pState)
{
	setButtonState(mCancelButton, pState, tr("Cancel"));
}


void WorkflowQtWidget::onCancelButtonClicked()
{
	setCancelButtonState(ButtonState::DISABLED);
	Q_EMIT fireUserCancelled();
}


void WorkflowQtWidget::setButtonState(QAbstractButton* pButton, ButtonState pState, const QString& pText)
{
	pButton->setText(pText);

	switch (pState)
	{
		case ButtonState::ENABLED:
			pButton->setVisible(true);
			pButton->setEnabled(true);
			break;

		case ButtonState::DISABLED:
			pButton->setVisible(true);
			pButton->setEnabled(false);
			break;

		case ButtonState::HIDDEN:
			pButton->setVisible(false);
			break;

		case ButtonState::FOCUSSED:
			pButton->setVisible(true);
			pButton->setEnabled(true);
			focusForwardButton();
			break;
	}
}


void WorkflowQtWidget::focusForwardButton()
{
	mForwardButton->setAutoDefault(true);
	mForwardButton->setDefault(true);
	mForwardButton->setFocus();
}
