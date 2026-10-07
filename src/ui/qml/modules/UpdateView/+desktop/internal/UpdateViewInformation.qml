/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

ColumnLayout {
	id: root

	property int downloadSize: -1
	property date releaseDate
	property alias version: textVersion.text

	GridLayout {
		Layout.fillWidth: true
		columnSpacing: Style.dimens.pane_spacing
		columns: 2

		GText {
			font.weight: Style.font.bold
			//: DESKTOP Information about the available, new version number.
			text: qsTr("New version")
		}
		GText {
			id: textVersion
		}
		GText {
			font.weight: Style.font.bold
			//: DESKTOP Date when the available update was released.
			text: qsTr("Release date")
		}
		GText {
			text: root.releaseDate.toLocaleDateString(Qt.locale(SettingsModel.language))
		}
		GText {
			font.weight: Style.font.bold
			//: DESKTOP Download size of the available update in megabyte.
			text: qsTr("Download size")
		}
		GText {
			text: "%1 MB".arg((root.downloadSize / 1000000).toLocaleString(Qt.locale(SettingsModel.language), "f", 1))
		}
	}
}
