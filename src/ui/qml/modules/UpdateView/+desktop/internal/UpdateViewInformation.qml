/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

ColumnLayout {
	id: root

	property string checksumUrl
	property int downloadSize: -1
	property string downloadUrl
	property string notesUrl
	property date releaseDate
	property alias version: textVersion.text

	GridLayout {
		Layout.fillWidth: true
		columnSpacing: Style.dimens.pane_spacing
		columns: 2

		GText {
			//: LABEL DESKTOP Information about the available, new version number.
			text: qsTr("New version:")
		}
		GText {
			id: textVersion

		}
		GText {
			//: LABEL DESKTOP Date when the available update was released.
			text: qsTr("Release date:")
		}
		GText {
			text: root.releaseDate.toLocaleDateString(Qt.locale(SettingsModel.language))
		}
		GText {
			//: LABEL DESKTOP Download size of the available update in megabyte.
			text: qsTr("Download size:")
		}
		GText {
			text: "%1 MiB".arg((root.downloadSize / 1024576).toLocaleString(Qt.locale(SettingsModel.language), "f", 1))
		}
		GText {

			//: LABEL DESKTOP Plaintext link to the update download.
			text: qsTr("Download link:")
			visible: root.downloadUrl !== ""
		}
		GText {
			text: "<a href=\"%1\">%1</a>".arg(root.downloadUrl)
			visible: root.downloadUrl !== ""
		}
		GText {
			//: LABEL DESKTOP Link to download checksum to verify the downloaded update file.
			text: qsTr("Checksum link:")
			visible: root.checksumUrl !== ""
		}
		GText {
			text: "<a href=\"%1\">%1</a>".arg(root.checksumUrl)
			visible: root.checksumUrl !== ""
		}
	}
}
