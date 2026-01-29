/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

ColumnLayout {
	id: root

	required property bool downloadInProgress
	required property int downloadProgressKB
	required property int downloadTotalKB
	required property string version

	signal abortDownload
	signal startDownload

	GText {
		//: DESKTOP %1 is replaced with the version number of the software update.
		text: qsTr("The update (version %1) is being performed...").arg(root.version)
		visible: root.downloadInProgress
	}
	GProgressBar {
		id: bar

		readonly property string localeDownloadedKB: root.downloadProgressKB.toLocaleString(Qt.locale(SettingsModel.language), "f", 0)
		readonly property string localeTotalKB: root.downloadTotalKB.toLocaleString(Qt.locale(SettingsModel.language), "f", 0)

		//: DESKTOP Name of an progress indicator during a download read by screen readers
		Accessible.name: qsTr("Download progress")
		Layout.fillWidth: true
		text: "%1 %".arg(Math.floor(bar.value))
		value: root.downloadProgressKB * 100 / root.downloadTotalKB
		visible: root.downloadInProgress
	}
	GButton {
		enabledTooltipText: SettingsModel.appUpdateData.url
		//: DESKTOP Start to download the update and execute it on Windows
		text: qsTr("Start update")
		visible: !root.downloadInProgress

		onActiveFocusChanged: if (activeFocus)
			Utils.positionViewAtItem(this)
		onClicked: root.startDownload()
	}
	GLink {
		colorStyle: Style.color.linkTitle
		font.underline: true
		horizontalPadding: 0
		//: DESKTOP Cancel the download of the update on Windows
		text: qsTr("Cancel update")
		visible: root.downloadInProgress

		onActiveFocusChanged: if (activeFocus)
			Utils.positionViewAtItem(this)
		onClicked: root.abortDownload()
	}
}
