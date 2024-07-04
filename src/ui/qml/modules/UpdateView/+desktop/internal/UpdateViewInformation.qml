/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

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
		columnSpacing: Constants.pane_spacing
		columns: 2

		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Information about the available, new version number.
			text: qsTr("New version:")

			FocusFrame {
			}
		}
		GText {
			id: textVersion

			activeFocusOnTab: true

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Date when the available update was released.
			text: qsTr("Release date:")

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			text: releaseDate.toLocaleDateString(Qt.locale(SettingsModel.language))

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Download size of the available update in megabyte.
			text: qsTr("Download size:")

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			text: "%1 MiB".arg((downloadSize / 1024576).toLocaleString(Qt.locale(SettingsModel.language), "f", 1))

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Plaintext link to the update download.
			text: qsTr("Download link:")
			visible: downloadUrl !== ""

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			text: "<a href=\"%1\">%1</a>".arg(downloadUrl)
			visible: downloadUrl !== ""

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Link to download checksum to verify the downloaded update file.
			text: qsTr("Checksum link:")
			visible: checksumUrl !== ""

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			text: "<a href=\"%1\">%1</a>".arg(checksumUrl)
			visible: checksumUrl !== ""

			FocusFrame {
			}
		}
	}
}
