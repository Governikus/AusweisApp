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
	required property int downloadProgressKiB
	required property int downloadTotalKiB

	signal toggleUpdate

	spacing: Style.dimens.pane_spacing

	GProgressBar {
		id: bar

		readonly property string localeDownloadedKB: (root.downloadProgressKiB * 1024 / 1000).toLocaleString(Qt.locale(SettingsModel.language), "f", 0)
		readonly property string localeTotalKB: (root.downloadTotalKiB * 1024 / 1000).toLocaleString(Qt.locale(SettingsModel.language), "f", 0)

		Layout.fillWidth: true
		text: "%1 KB / %2 KB".arg(localeDownloadedKB).arg(localeTotalKB)
		value: root.downloadProgressKiB * 100 / root.downloadTotalKiB
		visible: root.downloadInProgress

		//: LABEL DESKTOP %1 and %2 will be replaced with the already downloaded and the total file size.
		onRequestA11yUpdate: pValue => Accessible.name = qsTr("%1 of %2 Kilobyte downloaded").arg(localeDownloadedKB).arg(localeTotalKB)
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
