/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

RowLayout {
	id: root

	property alias progressText: bar.text
	property alias progressValue: bar.value
	property bool downloadInProgress: false

	signal skipUpdate()
	signal remindLater()
	signal toggleUpdate()

	spacing: Constants.component_spacing

	GProgressBar {
		id: bar

		visible: downloadInProgress

		Layout.fillWidth: true
		Layout.preferredHeight: ApplicationModel.scaleFactor * 40

		activeFocusOnTab: true
	}

	GButton {
		visible: !downloadInProgress

		//: LABEL DESKTOP_QML User choice to skip this update, the automatic update check will *not* inform about this update again.
		text: qsTr("Skip update")

		onClicked: root.skipUpdate()
	}

	GSpacer {
		visible: !downloadInProgress

		Layout.fillWidth: true
	}

	GButton {
		visible: !downloadInProgress

		//: LABEL DESKTOP_QML The available update is shown again after next automatic update check.
		text: qsTr("Remind me later")

		onClicked: root.remindLater()
	}

	GButton {
		text: Qt.platform.os === "osx"
			  //: LABEL DESKTOP_QML Open the Mac App Store on macOS
			  ? qsTr("Open App Store")
			  : downloadInProgress
			  //: LABEL DESKTOP_QML Cancel the download of the update on Windows
			  ? qsTr("Cancel update")
			  //: LABEL DESKTOP_QML Start to download the update and execute it on Windows
			  : qsTr("Start update")
		enabledTooltipText: SettingsModel.appUpdateData.url

		onClicked: root.toggleUpdate()
	}
}
