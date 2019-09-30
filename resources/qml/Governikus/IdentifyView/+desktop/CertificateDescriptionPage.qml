/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage
{
	signal exit()

	Accessible.name: qsTr("Self-authentication data view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the self-authentication data view of the AusweisApp2.") + SettingsModel.translationTrigger

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Provider Information")
		rootEnabled: false
		showHelp: false
	}

	Pane {
		id: pane

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Constants.pane_padding

		//: LABEL DESKTOP_QML
		title: qsTr("Provider Information") + SettingsModel.translationTrigger

		Repeater {
			id: listView

			model: certificateDescriptionModel

			LabeledText {
				width: parent.width

				activeFocusOnTab: true

				label: model.label
				text: model.text
				textFormat: Text.PlainText
			}
		}
	}

	GButton {
		id: button

		anchors.right: pane.right
		anchors.bottom: pane.bottom
		anchors.margins: Constants.pane_padding

		activeFocusOnTab: true
		Accessible.name: button.text

		//: LABEL DESKTOP_QML
		text: qsTr("Close") + SettingsModel.translationTrigger
		onClicked: parent.exit()
	}
}
