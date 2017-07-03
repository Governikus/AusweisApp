/*!
 * \brief Model implementation for the settings.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QObject>


namespace governikus
{

class SettingsModel
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool developerMode READ isDeveloperMode WRITE setDeveloperMode)
	Q_PROPERTY(bool useSelfauthenticationTestUri READ useSelfauthenticationTestUri WRITE setUseSelfauthenticationTestUri)

	public:
		bool isDeveloperMode() const;
		void setDeveloperMode(bool pEnabled);

		bool useSelfauthenticationTestUri() const;
		void setUseSelfauthenticationTestUri(bool pUse);
};

} /* namespace governikus */
