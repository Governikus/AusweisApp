/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
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
	property alias hintTitle: hintItem.title
	property alias icon: customIcon.source
	property alias subheader: subheader.text
	property alias text: resultText.text
	property alias textFormat: resultText.textFormat

	signal cancelClicked
	signal continueClicked
	signal hintClicked

	function confirm() {
		buttonContinue.clicked();
	}

	spacing: Constants.component_spacing

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: cancelClicked()
	}

	PaneTitle {
		id: paneTitle

		Layout.alignment: Qt.AlignHCenter
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

		Layout.maximumWidth: Number.POSITIVE_INFINITY
		spacing: Constants.pane_spacing

		GText {
			id: subheader

			textStyle: Style.text.subline
			visible: text !== ""
		}
		GText {
			id: resultText

			visible: text !== ""
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	Hint {
		id: hintItem

		Layout.fillWidth: true
		//: LABEL ANDROID IOS
		title: qsTr("Hint")
		visible: text !== ""

		onClicked: hintClicked()
		onReceivedFocus: pItem => baseItem.positionViewAtItem(pItem)
	}
	GButton {
		id: buttonContinue

		Layout.alignment: Qt.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("OK")
		tintIcon: true
		visible: text !== ""

		onClicked: continueClicked()
		onFocusChanged: if (focus)
			baseItem.positionViewAtItem(this)
	}
}
