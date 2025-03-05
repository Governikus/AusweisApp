/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.View
import Governikus.Type
import Governikus.Style

FlickableSectionPage {
	id: root

	spacing: Style.dimens.pane_padding

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	PaneTitle {
		Layout.alignment: Qt.AlignHCenter
		Layout.leftMargin: Style.dimens.pane_padding
		Layout.rightMargin: Style.dimens.pane_padding
		//: LABEL ANDROID IOS
		text: qsTr("Provider Information")
	}
	Repeater {
		model: CertificateDescriptionModel

		LabeledText {
			Layout.fillWidth: true
			Layout.leftMargin: Style.dimens.pane_padding
			Layout.rightMargin: Style.dimens.pane_padding
			textFormat: Text.PlainText
		}
	}
}
