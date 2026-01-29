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
	property string firstHintButtonLink
	property alias firstHintButtonText: hintItem.buttonText
	property alias firstHintText: hintItem.text
	property alias firstHintTitle: hintItem.title
	property alias header: resultHeader.text
	property alias hintBoxesTitle: hintBoxesTitle.text
	property string linkToOpen
	property alias mailButtonVisible: mailButton.visible
	property string popupText
	property string popupTitle
	default property alias resultData: resultContent.data
	property string secondHintButtonLink
	property alias secondHintButtonText: secondHintItem.buttonText
	property alias secondHintText: secondHintItem.text
	property alias secondHintTitle: secondHintItem.title
	property alias showOkButton: button.visible
	property alias subheader: subheader.text
	property alias text: resultText.text
	property alias textColor: resultText.color
	property alias textFormat: resultText.textFormat

	signal cancelClicked
	signal continueClicked
	signal emailButtonPressed
	signal firstHintClicked
	signal secondHintClicked

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

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		visible: text !== ""
	}
	GText {
		id: resultText

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		visible: text !== ""
	}
	ColumnLayout {
		id: resultContent

		Layout.alignment: Qt.AlignHCenter
	}
	RowLayout {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing
		visible: root.popupTitle !== "" || root.popupText !== ""

		GButton {
			id: mailButton

			icon.source: "qrc:///images/email_icon.svg"
			style: Style.color.controlOptional
			//: DESKTOP
			text: qsTr("Send email")
			tintIcon: true
			visible: false

			onClicked: root.emailButtonPressed()
		}
		GButton {
			icon.source: "qrc:/images/desktop/save_icon.svg"
			style: Style.color.controlOptional
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
		GButton {
			property ConfirmationPopup popup: null

			function destroyPopup() {
				if (popup) {
					popup.close();
					popup.destroy();
					popup = null;
				}
			}

			icon.source: "qrc:/images/info.svg"
			style: Style.color.controlOptional
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
	GSpacer {
		Layout.fillHeight: true
	}
	GText {
		id: hintBoxesTitle

		textStyle: Style.text.subline
		visible: text !== ""
	}
	Hint {
		id: hintItem

		Layout.fillWidth: true
		linkToOpen: root.firstHintButtonLink
		//: DESKTOP
		title: qsTr("Hint")
		visible: text !== ""

		onClicked: root.firstHintClicked()
	}
	Hint {
		id: secondHintItem

		Layout.fillWidth: true
		linkToOpen: root.secondHintButtonLink
		//: DESKTOP
		title: qsTr("Hint")
		visible: text !== ""

		onClicked: root.secondHintClicked()
	}
	GButton {
		id: button

		readonly property bool hasLink: root.linkToOpen !== ""

		Accessible.description: hasLink ? Utils.platformAgnosticLinkOpenText(root.linkToOpen, Accessible.name) : ""
		Accessible.role: hasLink ? Accessible.Link : Accessible.Button
		Layout.alignment: Qt.AlignHCenter
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
