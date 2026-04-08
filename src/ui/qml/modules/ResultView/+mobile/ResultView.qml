/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
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
	property alias header: paneTitle.text
	property alias hintBoxesTitle: pinResetHints.title
	property string hintButtonLink
	property alias hintButtonText: hintItem.buttonText
	property alias hintText: hintItem.text
	property alias hintTitle: hintItem.title
	property string linkToOpen
	property alias mailButtonText: mailButton.text
	default property alias resultData: layout.data
	property alias statusCode: pinResetHints.statusCode
	property alias subheader: subheader.text
	property alias text: resultText.text
	property alias textFormat: resultText.textFormat

	signal cancelClicked
	signal continueClicked
	signal linkAboutToOpen
	signal mailClicked

	function confirm() {
		buttonContinue.clicked();
	}

	spacing: Style.dimens.pane_spacing

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancelClicked()
	}

	QtObject {
		id: d

		property bool buttonCorrectionNeeded: !contentBetweenButtons && mailButton.visible
		property bool contentBetweenButtons: hintItem.visible || pinResetHints.visible
		property real maxButtonWidth: Math.max(buttonContinue.implicitWidth, mailButton.implicitWidth)
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

		Subheading {
			id: subheader

			visible: text !== ""
		}
		GText {
			id: resultText

			visible: text !== ""
		}
	}
	SecondaryButton {
		id: mailButton

		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: d.buttonCorrectionNeeded ? d.maxButtonWidth : implicitWidth
		Layout.preferredWidth: d.buttonCorrectionNeeded ? d.maxButtonWidth : implicitWidth
		icon.source: "qrc:///images/email_icon.svg"
		tintIcon: true
		visible: text !== ""

		onClicked: root.mailClicked()
	}
	Hint {
		id: hintItem

		Layout.fillWidth: true
		linkToOpen: root.hintButtonLink
		//: MOBILE
		title: qsTr("Hint")
		visible: text !== ""

		onLinkAboutToOpen: root.linkAboutToOpen()
	}
	PinResetHints {
		id: pinResetHints

		onLinkAboutToOpen: root.linkAboutToOpen()
	}
	GButton {
		id: buttonContinue

		readonly property bool hasLink: root.linkToOpen !== ""

		Accessible.description: hasLink ? Utils.platformAgnosticLinkOpenText(root.linkToOpen, Accessible.name) : ""
		Accessible.role: hasLink ? Accessible.Link : Accessible.Button
		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: d.buttonCorrectionNeeded ? d.maxButtonWidth : implicitWidth
		Layout.preferredWidth: d.buttonCorrectionNeeded ? d.maxButtonWidth : implicitWidth
		Layout.topMargin: d.buttonCorrectionNeeded ? -Style.dimens.pane_spacing + Style.dimens.text_spacing : 0

		//: MOBILE
		text: qsTr("OK")
		tintIcon: true
		visible: text !== ""

		onClicked: root.continueClicked()
	}
}
