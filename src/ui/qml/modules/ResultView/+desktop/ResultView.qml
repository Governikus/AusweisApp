/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
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
	default property alias data: resultContent.data
	property alias header: resultHeader.text
	property alias hintButtonText: hintItem.buttonText
	property alias hintText: hintItem.text
	property alias hintTitle: hintItem.title
	property alias mailButtonVisible: mailButton.visible
	property string popupText
	property string popupTitle
	property alias showOkButton: button.visible
	property alias subheader: subheader.text
	property alias text: resultText.text
	property alias textColor: resultText.color
	property alias textFormat: resultText.textFormat

	signal cancelClicked
	signal continueClicked
	signal emailButtonPressed
	signal hintClicked

	function confirm() {
		button.clicked();
	}

	spacing: Style.dimens.pane_spacing

	Keys.onEnterPressed: button.clicked()
	Keys.onEscapePressed: button.clicked()
	Keys.onReturnPressed: button.clicked()

	GText {
		id: resultHeader

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
		visible: text !== ""
	}
	WorkflowAnimationLoader {
		id: animatedIcon

		Layout.alignment: Qt.AlignHCenter
		animated: false
	}
	GText {
		id: subheader

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.subline
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
			//: LABEL DESKTOP
			text: qsTr("Send email")
			tintIcon: true
			visible: false

			onClicked: root.emailButtonPressed()
		}
		GButton {
			icon.source: "qrc:/images/desktop/save_icon.svg"
			style: Style.color.controlOptional
			//: LABEL DESKTOP
			text: qsTr("Save log")
			tintIcon: true

			onClicked: {
				LogModel.setLogFile(0);
				let filenameSuggestion = LogModel.createLogFileName(LogModel.getCurrentLogFileDate());
				fileDialog.selectFile(filenameSuggestion);
			}

			GFileDialog {
				id: fileDialog

				defaultSuffix: "log"
				//: LABEL DESKTOP
				nameFilters: qsTr("Logfiles (*.log)")

				//: LABEL DESKTOP
				title: qsTr("Save log")

				onAccepted: LogModel.saveCurrentLogFile(file)
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
			//: LABEL DESKTOP
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
	Hint {
		id: hintItem

		Layout.fillWidth: true
		//: LABEL DESKTOP
		title: qsTr("Hint")
		visible: text !== ""

		onClicked: root.hintClicked()
	}
	GButton {
		id: button

		Layout.alignment: Qt.AlignHCenter
		Layout.preferredHeight: height
		Layout.preferredWidth: width
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
