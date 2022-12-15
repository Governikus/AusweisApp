/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

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
			textStyle: Style.text.normal

			FocusFrame {
			}
		}
		GText {
			id: textVersion
			activeFocusOnTab: true
			textStyle: Style.text.normal

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Date when the available update was released.
			text: qsTr("Release date:")
			textStyle: Style.text.normal

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			text: releaseDate.toLocaleDateString(Qt.locale(SettingsModel.language))
			textStyle: Style.text.normal

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Download size of the available update in megabyte.
			text: qsTr("Download size:")
			textStyle: Style.text.normal

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			text: "%1 MiB".arg((downloadSize / 1024576).toLocaleString(Qt.locale(SettingsModel.language), "f", 1))
			textStyle: Style.text.normal

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Plaintext link to the update download.
			text: qsTr("Download link:")
			textStyle: Style.text.normal
			visible: downloadUrl !== ""

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			text: "<a href=\"%1\">%1</a>".arg(downloadUrl)
			textStyle: Style.text.normal
			visible: downloadUrl !== ""

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP Link to download checksum to verify the downloaded update file.
			text: qsTr("Checksum link:")
			textStyle: Style.text.normal
			visible: checksumUrl !== ""

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			text: "<a href=\"%1\">%1</a>".arg(checksumUrl)
			textStyle: Style.text.normal
			visible: checksumUrl !== ""

			FocusFrame {
			}
		}
	}
}
