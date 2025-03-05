/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View

FlickableSectionPage {
	id: root

	property alias animation: animatedIcon.animation
	property alias animationSymbol: animatedIcon.symbol
	property alias animationType: animatedIcon.type
	property alias buttonIcon: buttonContinue.icon.source
	property alias buttonLayoutDirection: buttonContinue.layoutDirection
	property alias buttonText: buttonContinue.text
	default property alias children: layout.data
	property alias header: paneTitle.text
	property alias hintButtonText: hintItem.buttonText
	property alias hintText: hintItem.text
	property alias hintTitle: hintItem.title
	property alias subheader: subheader.text
	property alias text: resultText.text
	property alias textFormat: resultText.textFormat

	signal cancelClicked
	signal continueClicked
	signal hintClicked

	function confirm() {
		buttonContinue.clicked();
	}

	spacing: Style.dimens.pane_spacing

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancelClicked()
	}

	PaneTitle {
		id: paneTitle

		Layout.alignment: Qt.AlignHCenter
	}
	WorkflowAnimationLoader {
		id: animatedIcon

		Layout.alignment: Qt.AlignHCenter
		animated: false
	}
	ColumnLayout {
		id: layout

		Layout.maximumWidth: Number.POSITIVE_INFINITY
		spacing: Style.dimens.pane_spacing

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

		onClicked: root.hintClicked()
	}
	GButton {
		id: buttonContinue

		Layout.alignment: Qt.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("OK")
		tintIcon: true
		visible: text !== ""

		onClicked: root.continueClicked()
	}
}
