/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.CertificateDescriptionModel 1.0


SectionPage
{
	signal exit()

	Accessible.name: qsTr("Self-authentication data view")
	Accessible.description: qsTr("This is the self-authentication data view of the AusweisApp2.")

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Provider Information")
		rootEnabled: false
		showHelp: false
	}

	ScrollablePane {
		id: pane

		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		activeFocusOnTab: true

		//: LABEL DESKTOP
		title: qsTr("Provider Information")

		Repeater {
			id: listView

			model: CertificateDescriptionModel

			LabeledText {
				width: parent.width

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

		//: LABEL DESKTOP
		text: qsTr("Close")
		onClicked: parent.exit()
	}
}
