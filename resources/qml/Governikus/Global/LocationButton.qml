/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


GRadioButton {
	id: root

	property string language
	property alias image: root.icon.source

	checked: SettingsModel.language === language
	onCheckedChanged: if (checked) SettingsModel.language = language
}
