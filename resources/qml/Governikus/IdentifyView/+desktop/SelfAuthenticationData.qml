/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.SelfAuthModel 1.0


SectionPage
{
	id: baseItem

	Accessible.name: qsTr("Self-authentication data view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the self-authentication data view of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onReturnPressed: okButton.onClicked()
	Keys.onEnterPressed: okButton.onClicked()
	Keys.onEscapePressed: okButton.onClicked()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Read data")
		rootEnabled: false
		showHelp: false
	}

	Row {
		id: statusRow

		height: parent.height / 4
		anchors.top: parent.top
		anchors.topMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		spacing: Constants.component_spacing

		StatusIcon {
			height: Style.dimens.status_icon_medium
			anchors.verticalCenter: parent.verticalCenter

			source: "qrc:///images/status_ok.svg"
		}

		GText {
			id: successText

			anchors.verticalCenter: parent.verticalCenter

			activeFocusOnTab: true
			Accessible.name: successText.text

			//: INFO DESKTOP_QML Status message that the self authentication successfully completed.
			text: qsTr("Successfully read data") + SettingsModel.translationTrigger
			textStyle: Style.text.header

			FocusFrame {}
		}
	}

	Pane {
		id: pane

		anchors.top: statusRow.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Constants.pane_padding

		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		title: qsTr("Read data") + SettingsModel.translationTrigger

		Grid {
			id: grid

			width: parent.width

			columns: 3
			spacing: Constants.groupbox_spacing
			verticalItemAlignment: Grid.AlignTop

			Repeater {
				id: dataRepeater
				model: SelfAuthModel

				LabeledText {
					width: (pane.width - 2 * Constants.pane_padding - (grid.columns - 1) * grid.spacing) / grid.columns

					activeFocusOnTab: true

					label: name
					text: value === "" ? "---" : value
				}
			}
		}

		GButton {
			id: okButton

			anchors.right: parent.right

			activeFocusOnTab: true
			Accessible.name: okButton.text

			//: LABEL DESKTOP_QML
			text: qsTr("OK") + SettingsModel.translationTrigger
			onClicked: baseItem.nextView(SectionPage.Views.Main)
		}
	}
}
