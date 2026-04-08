/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQml
import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	property alias animation: animatedIcon.animation
	property alias animationSymbol: animatedIcon.symbol
	property alias animationType: animatedIcon.type
	property alias buttonIcon: button.icon.source
	property alias buttonLayoutDirection: button.layoutDirection
	property alias buttonText: button.text
	property alias header: resultHeader.text
	property alias hintBoxesTitle: pinResetHints.title
	property string hintButtonLink
	property alias hintButtonText: hintItem.buttonText
	property alias hintText: hintItem.text
	property alias hintTitle: hintItem.title
	property string linkToOpen
	property alias mailButtonVisible: mailButton.visible
	property string popupText
	property string popupTitle
	default property alias resultData: resultContent.data
	property alias showOkButton: button.visible
	property alias statusCode: pinResetHints.statusCode
	property alias subheader: subheader.text
	property alias text: resultText.text
	property alias textColor: resultText.color
	property alias textFormat: resultText.textFormat

	signal cancelClicked
	signal continueClicked
	signal emailButtonPressed
	signal linkAboutToOpen

	function confirm() {
		button.clicked();
	}

	spacing: Style.dimens.pane_spacing

	Heading {
		id: resultHeader

		visible: text !== ""
	}
	WorkflowAnimationLoader {
		id: animatedIcon

		Layout.alignment: Qt.AlignHCenter
		animated: false
	}
	Subheading {
		id: subheader

		Layout.alignment: Style.scanPatternAlignment
		horizontalAlignment: Style.scanPatternAlignment
		visible: text !== ""
	}
	GText {
		id: resultText

		Layout.alignment: Style.scanPatternAlignment
		horizontalAlignment: Style.scanPatternAlignment
		visible: text !== ""
	}
	ColumnLayout {
		id: resultContent

		Layout.alignment: Style.scanPatternAlignment
		visible: children.length !== 0
	}
	RowLayout {
		Layout.alignment: Style.scanPatternAlignment
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing
		visible: root.popupTitle !== "" || root.popupText !== ""

		SecondaryButton {
			id: mailButton

			icon.source: "qrc:///images/email_icon.svg"
			//: DESKTOP
			text: qsTr("Send email")
			tintIcon: true
			visible: false

			onClicked: root.emailButtonPressed()
		}
		SecondaryButton {
			icon.source: "qrc:/images/desktop/save_icon.svg"
			//: DESKTOP
			text: qsTr("Save log")
			tintIcon: true

			onClicked: {
				let filenameSuggestion = logModel.createLogFileName();
				fileDialog.selectFile(filenameSuggestion);
			}

			GFileDialog {
				id: fileDialog

				defaultSuffix: "log"
				//: DESKTOP
				nameFilters: qsTr("Logfiles (*.log)")

				//: DESKTOP
				title: qsTr("Save log")

				onAccepted: logModel.saveLogFile(selectedFile, true)
			}
			LogModel {
				id: logModel

			}
		}
		SecondaryButton {
			property ConfirmationPopup popup: null

			function destroyPopup() {
				if (popup) {
					popup.close();
					popup.destroy();
					popup = null;
				}
			}

			icon.source: "qrc:/images/info.svg"
			//: DESKTOP
			text: qsTr("See details")
			tintIcon: true
			visible: root.popupTitle !== "" || root.popupText !== ""

			onClicked: {
				destroyPopup();
				popup = detailedResultPopup.createObject(root, {
					text: root.popupText,
					title: root.popupTitle
				});
				popup.open();
			}
			onVisibleChanged: destroyPopup()
		}
	}
	Hint {
		id: hintItem

		Layout.fillWidth: true
		linkToOpen: root.hintButtonLink
		//: DESKTOP
		title: qsTr("Hint")
		visible: text !== ""

		onLinkAboutToOpen: root.linkAboutToOpen()
	}
	PinResetHints {
		id: pinResetHints

		onLinkAboutToOpen: root.linkAboutToOpen()
	}
	GButton {
		id: button

		readonly property bool hasLink: root.linkToOpen !== ""

		Accessible.description: hasLink ? Utils.platformAgnosticLinkOpenText(root.linkToOpen, Accessible.name) : ""
		Accessible.role: hasLink ? Accessible.Link : Accessible.Button
		Layout.alignment: Style.scanPatternAlignment
		Layout.preferredHeight: height
		Layout.preferredWidth: width
		enabledTooltipText: hasLink ? root.linkToOpen : ""
		text: qsTr("OK")
		tintIcon: true
		visible: text !== ""

		onClicked: {
			root.leaveView();
			root.continueClicked();
		}
	}
	Component {
		id: detailedResultPopup

		ConfirmationPopup {
			style: ConfirmationPopup.PopupStyle.OkButton
		}
	}
}
