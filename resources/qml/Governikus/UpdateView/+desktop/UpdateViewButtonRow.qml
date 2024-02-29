/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel

RowLayout {
	id: root

	property bool downloadInProgress: false
	property alias progressText: bar.text
	property alias progressValue: bar.value

	signal toggleUpdate

	spacing: Constants.component_spacing

	GProgressBar {
		id: bar

		Layout.fillWidth: true
		activeFocusOnTab: true
		visible: downloadInProgress
	}
	GSpacer {
		Layout.fillWidth: true
		visible: !downloadInProgress
	}
	GButton {
		enabledTooltipText: SettingsModel.appUpdateData.url
		text: Qt.platform.os === "osx" ?
		//: LABEL DESKTOP Open the Mac App Store on macOS
		qsTr("Open App Store") : downloadInProgress ?
		//: LABEL DESKTOP Cancel the download of the update on Windows
		qsTr("Cancel update") :
		//: LABEL DESKTOP Start to download the update and execute it on Windows
		qsTr("Start update")

		onClicked: root.toggleUpdate()
	}
}
