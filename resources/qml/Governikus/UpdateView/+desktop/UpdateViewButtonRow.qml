/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
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

		activeFocusOnTab: true

		backgroundColor: Style.color.background_pane_inactive
	}

	GButton {
		visible: !downloadInProgress

		//: LABEL DESKTOP User choice to skip this update, the automatic update check will *not* inform about this update again.
		text: qsTr("Skip update")

		onClicked: root.skipUpdate()
	}

	GSpacer {
		visible: !downloadInProgress

		Layout.fillWidth: true
	}

	GButton {
		visible: !downloadInProgress

		//: LABEL DESKTOP The available update is shown again after next automatic update check.
		text: qsTr("Remind me later")

		onClicked: root.remindLater()
	}

	GButton {
		text: Qt.platform.os === "osx"
			  //: LABEL DESKTOP Open the Mac App Store on macOS
			  ? qsTr("Open App Store")
			  : downloadInProgress
			  //: LABEL DESKTOP Cancel the download of the update on Windows
			  ? qsTr("Cancel update")
			  //: LABEL DESKTOP Start to download the update and execute it on Windows
			  : qsTr("Start update")
		enabledTooltipText: SettingsModel.appUpdateData.url

		onClicked: root.toggleUpdate()
	}
}
