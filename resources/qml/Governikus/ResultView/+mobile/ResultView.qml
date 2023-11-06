/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View

FlickableSectionPage {
	id: baseItem

	property alias buttonIcon: buttonContinue.icon.source
	property alias buttonText: buttonContinue.text
	default property alias children: layout.data
	property alias header: paneTitle.text
	property alias hintButtonText: hintItem.buttonText
	property alias hintText: hintItem.text
	property alias icon: customIcon.source
	property alias text: resultText.text
	property alias textFormat: resultText.textFormat

	signal cancelClicked
	signal continueClicked
	signal hintClicked

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: cancelClicked()
	}

	TintableIcon {
		id: customIcon

		Layout.alignment: Qt.AlignHCenter
		sourceSize.height: Style.dimens.header_icon_size
		tintEnabled: false
		visible: source.toString() !== ""
	}
	ColumnLayout {
		id: layout

		Layout.margins: Constants.pane_padding
		spacing: Constants.pane_spacing

		PaneTitle {
			id: paneTitle

		}
		GText {
			id: resultText

			Layout.alignment: Qt.AlignCenter | Qt.AlignTop
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.topMargin: Constants.component_spacing
			visible: text !== ""
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	Hint {
		id: hintItem

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.maximumWidth: Style.dimens.max_text_width
		Layout.topMargin: Constants.component_spacing
		visible: text !== ""

		onClicked: hintClicked()
	}
	GButton {
		id: buttonContinue

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.component_spacing

		//: LABEL ANDROID IOS
		text: qsTr("OK")
		tintIcon: true
		visible: text !== ""

		onClicked: continueClicked()
	}
}
