/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

ColumnLayout {
	id: root

	spacing: Style.dimens.pane_spacing

	GPane {
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing

		GMenuItem {
			Layout.fillWidth: true
			buttonIconSource: "qrc:///images/open_website.svg"
			//: LABEL DESKTOP
			buttonText: qsTr("Open website")
			buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language)
			iconSource: "qrc:/images/desktop/privacy_icon.svg"

			//: LABEL DESKTOP
			title: qsTr("Privacy statement")

			onClicked: Qt.openUrlExternally(buttonTooltip)
		}
		GSeparator {
			Layout.fillWidth: true
		}
		GMenuItem {
			Layout.fillWidth: true
			buttonIconSource: "qrc:///images/open_website.svg"
			//: LABEL DESKTOP
			buttonText: qsTr("Open website")
			buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language)
			iconSource: "qrc:/images/desktop/a11y_icon.svg"

			//: LABEL DESKTOP
			title: qsTr("Accessibility statement")

			onClicked: Qt.openUrlExternally(buttonTooltip)
		}
	}
	Item {
		id: baseItem

		Layout.fillWidth: true
		Layout.preferredHeight: column.implicitHeight

		GPane {
			id: column

			anchors.fill: parent
			spacing: Style.dimens.pane_spacing

			Repeater {
				id: repeater

				model: VersionInformationModel

				delegate: LabeledText {
					required property string key
					required property string value

					label: key
					text: value
					width: baseItem.width

					Accessible.onPressAction: mouseArea.clicked(null)
					Keys.onSpacePressed: mouseArea.clicked(null)
					onFocusChanged: if (focus)
						Utils.positionViewAtItem(this)
				}
			}
		}
		MouseArea {
			id: mouseArea

			property int counter: 0

			anchors.fill: parent

			onClicked: {
				counter += 1;
				if (counter === 10) {
					SettingsModel.developerOptions = !SettingsModel.developerOptions;
					ApplicationModel.showFeedback((SettingsModel.developerOptions ?
						//: LABEL DESKTOP
						qsTr("Developer options activated.") :
						//: LABEL DESKTOP
						qsTr("Developer options deactivated.")));
					counter = 0;
				}
			}
		}
	}
}
