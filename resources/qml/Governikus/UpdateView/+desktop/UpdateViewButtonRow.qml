/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0


RowLayout {
	id: root

	property alias downloadTooltip: downloadButton.enabledTooltipText

	signal skipUpdate()
	signal remindLater()
	signal downloadUpdate()

	GButton {
		//: LABEL DESKTOP_QML User choice to skip this update, the automatic update check will *not* inform about this update again.
		text: qsTr("Skip update") + SettingsModel.translationTrigger

		onClicked: root.skipUpdate()
	}

	Item {
		id: stretchSpacer

		Layout.fillWidth: true
	}

	GButton {
		//: LABEL DESKTOP_QML The available update is shown again after next automatic update check.
		text: qsTr("Remind me later") + SettingsModel.translationTrigger

		onClicked: root.remindLater()
	}

	GButton {
		id: downloadButton

		//: LABEL DESKTOP_QML Download the update, opens the download link in the user's default browser.
		text: qsTr("Download update") + SettingsModel.translationTrigger

		onClicked: root.downloadUpdate()
	}
}
