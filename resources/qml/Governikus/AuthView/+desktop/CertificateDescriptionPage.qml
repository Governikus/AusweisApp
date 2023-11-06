/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.CertificateDescriptionModel

SectionPage {
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

		Repeater {
			id: listView

			model: CertificateDescriptionModel

			LabeledText {
				Layout.fillWidth: true
				alignment: Text.AlignHCenter
				label: model.label
				text: model.text
				textFormat: Text.PlainText
			}
		}
	}
}
