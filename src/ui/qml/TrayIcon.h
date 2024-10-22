/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include <QIcon>
#include <QObject>

#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	#include <QSystemTrayIcon>
#endif


namespace governikus
{

class TrayIcon
	: public QObject
{
	Q_OBJECT

	private:
		QIcon mIcon;
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
		QSystemTrayIcon* mTrayIcon;

	private Q_SLOTS:
		void onActivated(QSystemTrayIcon::ActivationReason pReason);
		void updateMenu();
#endif

	public Q_SLOTS:
		void onTranslationChanged();

	public:
		TrayIcon();
		~TrayIcon() override;

		[[nodiscard]] const QIcon& getIcon() const;
		void create();
		void shutdown();
		void setVisible(bool pVisible);

		void showMessage(const QString& pTitle, const QString& pMessage);

	Q_SIGNALS:
		void fireShow();
		void fireQuit();
};

} // namespace governikus
