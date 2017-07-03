/*!
 * GuiProfile.h
 *
 * \brief Singleton GuiProfile specifies platform specific customizations the
 * GUI.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"

#include <QSize>
#include <QString>

namespace governikus
{

defineEnumType(ScreenSize,
		SMALL, MEDIUM, LARGE
		)

class GuiProfile
{
	public:
		GuiProfile();
		~GuiProfile();

	private:
		static GuiProfile mProfile;

		ScreenSize mScreenSize;
		bool mShowWindow;
		bool mShowMainTabs;
		QSize mWindowSize;
		QString mDebugStyleSheet;

	public:
		static GuiProfile& getProfile()
		{
			return mProfile;
		}


		ScreenSize getScreenSize() const;
		QSize getWindowSize() const;

		void setDebugStyleSheet(const QString& pStyleSheet);
		void setScreenSize(ScreenSize pScreenSize);
		void setShowMainTabs(bool pShow);
		void setShowWindow(bool pShow);
		void setWindowSize(QSize pWindowSize);

		bool isSmallScreen() const;
		bool isMediumScreen() const;
		bool isLargeScreen() const;

		bool getShowWindow() const;
		bool getShowMainTabs() const;
		const QString& getDebugStyleSheet() const;
		QLatin1String getStyleSheetName();
};

} /* namespace governikus */
