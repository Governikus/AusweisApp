/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	spacing: Constants.pane_padding

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	PaneTitle {
		Layout.alignment: Qt.AlignHCenter
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		//: LABEL ANDROID IOS
		text: qsTr("Provider Information")
	}
	Repeater {
		model: CertificateDescriptionModel

		LabeledText {
			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			label: model.label
			text: model.text
			textFormat: Text.PlainText
		}
	}
}
