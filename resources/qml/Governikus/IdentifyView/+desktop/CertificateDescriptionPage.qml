import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage
{
	signal exit()

	titleBarAction: TitleBarAction {
		text: qsTr("Provider Information")
		showSettings: false
		showHelp: false
	}

	Pane {
		id: pane
		anchors.top: parent.top
		anchors.margins: Constants.pane_padding
		title: qsTr("Provider Information") + settingsModel.translationTrigger

		Repeater {
			id: listView
			model: certificateDescriptionModel

			LabeledText {
				id: delegate
				label: model.label
				text: model.text
				textFormat: Text.PlainText
				width: parent.width
			}
		}
	}

	GButton {
		id: button
		anchors.right: pane.right
		anchors.bottom: pane.bottom
		anchors.margins: Constants.pane_padding
		text: qsTr("Close")
		onClicked: parent.exit()
	}
}
