/*!
 * \brief Singleton GuiProfile specifies platform specific customizations the
 * GUI.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

namespace governikus
{

class GuiProfile
{
	private:
		static GuiProfile mProfile;
		GuiProfile();
		~GuiProfile() = default;

		bool mShowWindow;

	public:
		static GuiProfile& getProfile()
		{
			return mProfile;
		}


		void setShowWindow(bool pShow);
		bool getShowWindow() const;
};

} /* namespace governikus */
