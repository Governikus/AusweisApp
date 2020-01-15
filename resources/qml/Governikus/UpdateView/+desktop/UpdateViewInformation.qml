/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0


ColumnLayout {
	id: root

	property date releaseDate
	property alias version: textVersion.text
	property string downloadUrl
	property string checksumUrl
	property string notesUrl
	property int downloadSize: -1

	GridLayout {
		Layout.fillWidth: true

		columns: 2
		columnSpacing: Constants.pane_spacing

		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP_QML Information about the available, new version number.
			text: qsTr("New version:") + SettingsModel.translationTrigger
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			id: textVersion

			activeFocusOnTab: true

			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP_QML Date when the available update was released.
			text: qsTr("Release date:") + SettingsModel.translationTrigger
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			activeFocusOnTab: true

			text: releaseDate.toLocaleDateString(Qt.locale(SettingsModel.language))
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP_QML Download size of the available update in megabyte.
			text: qsTr("Download size:") + SettingsModel.translationTrigger
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			activeFocusOnTab: true

			text: (downloadSize / 1e6).toFixed(3) + " MB"
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			visible: downloadUrl !== ""

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML Plaintext link to the update download.
			text: qsTr("Download link:") + SettingsModel.translationTrigger
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			visible: downloadUrl !== ""

			activeFocusOnTab: true

			text: "<a href=\"%1\">%1</a>".arg(downloadUrl)
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			visible: checksumUrl !== ""

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML Link to download checksum to verify the downloaded update file.
			text: qsTr("Checksum link:") + SettingsModel.translationTrigger
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			visible: checksumUrl !== ""

			activeFocusOnTab: true

			text: "<a href=\"%1\">%1</a>".arg(checksumUrl)
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}
	}
}
