/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "TrayIcon.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "Env.h"

#include <QCoreApplication>

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	#include <QMenu>
#endif


using namespace governikus;


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason pReason)
{
	#ifdef Q_OS_MACOS
	Q_UNUSED(pReason)
	#else
	if (pReason == QSystemTrayIcon::Trigger)
	{
		Q_EMIT fireShow();
	}
	#endif
}


#endif


TrayIcon::TrayIcon()
	: QObject()
#ifdef Q_OS_MACOS
	, mIcon(QStringLiteral(":/images/macos/appIcon.svg"))
#else
	, mIcon(QStringLiteral(":/images/npa.svg"))
#endif
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	, mTrayIcon(nullptr)
#endif
{
}


TrayIcon::~TrayIcon()
{
	shutdown();
}


const QIcon& TrayIcon::getIcon() const
{
	return mIcon;
}


void TrayIcon::create()
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (!QSystemTrayIcon::isSystemTrayAvailable())
	{
		return;
	}

	mTrayIcon = new QSystemTrayIcon(mIcon);
	connect(mTrayIcon, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);
	connect(mTrayIcon, &QSystemTrayIcon::messageClicked, this, &TrayIcon::fireShow);

	updateMenu();

	mTrayIcon->setToolTip(QCoreApplication::applicationName());

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (generalSettings.showTrayIcon())
	{
		mTrayIcon->show();
	}

	if (!generalSettings.isAutoStart())
	{
		//: LABEL DESKTOP
		Env::getSingleton<ApplicationModel>()->showFeedback(tr("Application was started."));
	}
#endif
}


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
void TrayIcon::updateMenu()
{
	if (!mTrayIcon)
	{
		return;
	}

	QMenu* trayIconMenu = mTrayIcon->contextMenu();
	if (!trayIconMenu)
	{
		trayIconMenu = new QMenu(nullptr);
		mTrayIcon->setContextMenu(trayIconMenu);
	}

	trayIconMenu->clear();

	#if defined(Q_OS_MACOS)
	//: LABEL DESKTOP
	QAction* showApplicationAction = new QAction(tr("Open"), trayIconMenu);
	connect(showApplicationAction, &QAction::triggered, this, &TrayIcon::fireShow);
	trayIconMenu->addAction(showApplicationAction);
	trayIconMenu->addSeparator();
	#endif

	//: LABEL DESKTOP
	const auto quitAction = new QAction(tr("Quit %1").arg(QCoreApplication::applicationName()), trayIconMenu);
	connect(quitAction, &QAction::triggered, this, &TrayIcon::fireQuit);
	trayIconMenu->addAction(quitAction);
}


#endif

void TrayIcon::onTranslationChanged()
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	updateMenu();
#endif
}


void TrayIcon::shutdown()
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mTrayIcon != nullptr)
	{
		const QMenu* menu = mTrayIcon->contextMenu();
		if (menu != nullptr)
		{
			qDeleteAll(menu->actions());
			delete menu;
		}

		delete mTrayIcon;
		mTrayIcon = nullptr;
	}
#endif
}


void TrayIcon::setVisible(bool pVisible)
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mTrayIcon)
	{
		mTrayIcon->setVisible(pVisible);
	}
#else
	Q_UNUSED(pVisible)
#endif
}


bool TrayIcon::isVisible() const
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mTrayIcon)
	{
		mTrayIcon->isVisible();
	}
#endif
	return false;
}


void TrayIcon::showMessage(const QString& pTitle, const QString& pMessage)
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mTrayIcon)
	{
		mTrayIcon->showMessage(pTitle, pMessage, mIcon, 3000);
	}
#else
	Q_UNUSED(pTitle)
	Q_UNUSED(pMessage)
#endif
}
