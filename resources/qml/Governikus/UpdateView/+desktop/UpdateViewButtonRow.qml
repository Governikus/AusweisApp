/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

RowLayout {
	id: root

	property bool downloadInProgress: false
	property alias progressText: bar.text
	property alias progressValue: bar.value

	signal remindLater
	signal skipUpdate
	signal toggleUpdate

	spacing: Constants.component_spacing

	GProgressBar {
		id: bar
		Layout.fillWidth: true
		activeFocusOnTab: true
		backgroundColor: Style.color.background_pane_inactive
		visible: downloadInProgress
	}
	GButton {

		//: LABEL DESKTOP User choice to skip this update, the automatic update check will *not* inform about this update again.
		text: qsTr("Skip update")
		visible: !downloadInProgress

		onClicked: root.skipUpdate()
	}
	GSpacer {
		Layout.fillWidth: true
		visible: !downloadInProgress
	}
	GButton {

		//: LABEL DESKTOP The available update is shown again after next automatic update check.
		text: qsTr("Remind me later")
		visible: !downloadInProgress

		onClicked: root.remindLater()
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
