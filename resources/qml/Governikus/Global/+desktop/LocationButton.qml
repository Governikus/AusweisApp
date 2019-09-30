/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


GButton {
	id: root

	property string language
	property alias image: root.icon.source

	cursorShape: SettingsModel.language === language ? Qt.ArrowCursor : Qt.PointingHandCursor
	opacity: SettingsModel.language === language ? 1.0 : 0.5
	onClicked:  SettingsModel.language = language
}
