/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "TrayIcon.h"

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
	, mIcon(QStringLiteral(":/images/npa.svg"))
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

	const auto trayIconMenu = new QMenu(nullptr);

#if defined(Q_OS_MACOS)
	//: LABEL DESKTOP
	QAction* showApplicationAction = new QAction(tr("Open"), trayIconMenu);
	connect(showApplicationAction, &QAction::triggered, this, &TrayIcon::fireShow);
	trayIconMenu->addAction(showApplicationAction);
	trayIconMenu->addSeparator();
#endif

	//: LABEL DESKTOP
	const auto quitAction = new QAction(tr("Exit AusweisApp2"), trayIconMenu);
	connect(quitAction, &QAction::triggered, this, &TrayIcon::fireQuit);
	trayIconMenu->addAction(quitAction);

	mTrayIcon = new QSystemTrayIcon(mIcon);
	connect(mTrayIcon, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);
	connect(mTrayIcon, &QSystemTrayIcon::messageClicked, this, &TrayIcon::fireShow);

	mTrayIcon->setContextMenu(trayIconMenu);
	mTrayIcon->setToolTip(QCoreApplication::applicationName());

	mTrayIcon->show();
	//: LABEL DESKTOP
	showMessage(QCoreApplication::applicationName(), tr("Application was started."));
#endif
}


void TrayIcon::shutdown()
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mTrayIcon != nullptr)
	{
		QMenu* menu = mTrayIcon->contextMenu();
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


void TrayIcon::hide()
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mTrayIcon)
	{
		mTrayIcon->hide();
	}
#endif
}


void TrayIcon::showMessage(const QString& pTitle, const QString& pMessage)
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mTrayIcon)
	{
		mTrayIcon->showMessage(pTitle, pMessage, mIcon, 3000);
	}
#else
	Q_UNUSED(pTitle);
	Q_UNUSED(pMessage);
#endif
}
