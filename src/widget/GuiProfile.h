/*!
 * \brief Singleton GuiProfile specifies platform specific customizations the
 * GUI.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QSize>
#include <QString>

namespace governikus
{

class GuiProfile
{
	public:
		GuiProfile();
		~GuiProfile();

	private:
		static GuiProfile mProfile;

		bool mShowWindow;
		QString mDebugStyleSheet;

	public:
		static GuiProfile& getProfile()
		{
			return mProfile;
		}


		void setDebugStyleSheet(const QString& pStyleSheet);
		void setShowWindow(bool pShow);

		bool getShowWindow() const;
		const QString& getDebugStyleSheet() const;
		QLatin1String getStyleSheetName();
};

} /* namespace governikus */
