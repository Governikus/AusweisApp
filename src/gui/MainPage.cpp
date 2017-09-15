/*!
 * MainPage.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "MainPage.h"

#include <QPainter>
#include <QStyleOption>

#include "generic/LayoutBuilder.h"
#include "generic/TabButtonGroup.h"
#include "GuiProfile.h"

using namespace governikus;

MainPage::MainPage(QWidget* pParent)
	: Page(QCoreApplication::applicationName(), pParent)
	, mSelfInfoButton(new TabButton)
	, mBookmarksButton(new TabButton)
	, mHistoryButton(new TabButton)
	, mSettingsButton(new TabButton)
{
	setObjectName(QStringLiteral("MainPage"));

	setSideNavigationPage(true);

	TabButtonGroup* mainNavigationGroup = new TabButtonGroup;
	mainNavigationGroup->setObjectName(QStringLiteral("mainNavigationGroup"));
	if (GuiProfile::getProfile().isSmallScreen())
	{
		LayoutBuilder::HBox<>(mainNavigationGroup, 0, 0);
	}
	else
	{
		LayoutBuilder::VBox<>(mainNavigationGroup, 0, 0);
	}

	LayoutBuilder::VBox<>(this, 0, 0).add(mainNavigationGroup);

	struct ButtonInfo
	{
		QToolButton* mButton;
		const char* mOnIcon;
		const char* mOffIcon;
		QString mText;
	};

	ButtonInfo buttonInfos[] = {
		{mSelfInfoButton, ":/images/bt_1b.svg", ":/images/bt_1b.svg", tr("Identify")},
		{mBookmarksButton, ":/images/bt_2b.svg", ":/images/bt_2b.svg", tr("Provider")},
		{mHistoryButton, ":/images/bt_3b.svg", ":/images/bt_3b.svg", tr("History")},
		{mSettingsButton, ":/images/bt_4b.svg", ":/images/bt_4b.svg", tr("Settings")}
	};

	for (size_t i = 0; i < sizeof(buttonInfos) / sizeof(buttonInfos[0]); ++i)
	{
		const ButtonInfo& buttonInfo = buttonInfos[i];
		mainNavigationGroup->layout()->addWidget(buttonInfo.mButton);
		mainNavigationGroup->addButton(buttonInfo.mButton);

		buttonInfo.mButton->setText(buttonInfo.mText);
		buttonInfo.mButton->setCheckable(true);

		if (GuiProfile::getProfile().isSmallScreen())
		{
			buttonInfo.mButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		}
		else
		{
			buttonInfo.mButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		}

		QIcon icon;
		icon.addPixmap(QPixmap(QString::fromLatin1(buttonInfo.mOnIcon)), QIcon::Normal, QIcon::Off);
		icon.addPixmap(QPixmap(QString::fromLatin1(buttonInfo.mOffIcon)), QIcon::Normal, QIcon::On);
		buttonInfo.mButton->setIcon(icon);
	}
}


MainPage::~MainPage()
{
}


void MainPage::setNavigationEnabled(bool pEnabled)
{
	mSelfInfoButton->setEnabled(pEnabled);
	mBookmarksButton->setEnabled(pEnabled);
	mHistoryButton->setEnabled(pEnabled);
	mSettingsButton->setEnabled(pEnabled);
}


QAbstractButton* MainPage::getSelfInfoButton() const
{
	return mSelfInfoButton;
}


QAbstractButton* MainPage::getHistoryButton() const
{
	return mHistoryButton;
}


QAbstractButton* MainPage::getBookmarksButton() const
{
	return mBookmarksButton;
}


QAbstractButton* MainPage::getSettingsButton() const
{
	return mSettingsButton;
}


void MainPage::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
