/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

RowLayout {
	id: root

	property bool downloadInProgress: false
	property alias progressText: bar.text
	property alias progressValue: bar.value

	signal toggleUpdate

	spacing: Style.dimens.pane_spacing

	GProgressBar {
		id: bar

		Layout.fillWidth: true
		activeFocusOnTab: true
		visible: root.downloadInProgress
	}
	GSpacer {
		Layout.fillWidth: true
		visible: !root.downloadInProgress
	}
	GButton {
		enabledTooltipText: SettingsModel.appUpdateData.url
		text: root.downloadInProgress ?
		//: LABEL DESKTOP Cancel the download of the update on Windows
		qsTr("Cancel update") :
		//: LABEL DESKTOP Start to download the update and execute it on Windows
		qsTr("Start update")

		onClicked: root.toggleUpdate()
	}
}
