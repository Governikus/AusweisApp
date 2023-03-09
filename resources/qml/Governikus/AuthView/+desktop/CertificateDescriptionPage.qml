/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.CertificateDescriptionModel 1.0

SectionPage {
	signal exit

	titleBarAction: TitleBarAction {
		rootEnabled: false
		showHelp: false
		//: LABEL DESKTOP
		text: qsTr("Provider Information")
	}

	ScrollablePane {
		id: pane
		activeFocusOnTab: true
		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		//: LABEL DESKTOP
		title: qsTr("Provider Information")

		Repeater {
			id: listView
			model: CertificateDescriptionModel

			LabeledText {
				label: model.label
				text: model.text
				textFormat: Text.PlainText
				width: parent.width
			}
		}
	}
	GButton {
		id: button
		anchors.bottom: pane.bottom
		anchors.margins: Constants.pane_padding
		anchors.right: pane.right

		//: LABEL DESKTOP
		text: qsTr("Close")

		onClicked: parent.exit()
	}
}
