/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Global
import Governikus.TitleBar
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.LogModel
import Governikus.Type.UiModule

FlickableSectionPage {
	id: baseItem

	property alias header: resultHeader.text
	property alias hintButtonText: hintItem.buttonText
	property alias hintText: hintItem.text
	property alias icon: headerIcon.source
	property alias mailButtonVisible: mailButton.visible
	property string popupText
	property string popupTitle
	property alias showOkButton: button.visible
	property alias text: resultText.text
	property alias textColor: resultText.color

	signal emailButtonPressed
	signal hintClicked

	spacing: Constants.pane_spacing

	Keys.onEnterPressed: button.onClicked()
	Keys.onEscapePressed: button.onClicked()
	Keys.onReturnPressed: button.onClicked()

	TintableIcon {
		id: headerIcon

		Layout.alignment: Qt.AlignHCenter
		sourceSize.height: Style.dimens.header_icon_size
		tintEnabled: false
		visible: source.toString() !== ""
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GText {
		id: resultHeader

		Layout.alignment: Qt.AlignHCenter
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
		visible: text !== ""

		Accessible.onIgnoredChanged: baseItem.onVisibleChanged()

		FocusFrame {
		}
	}
	GText {
		id: resultText

		Layout.alignment: Qt.AlignHCenter
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		visible: text !== ""

		Accessible.onIgnoredChanged: baseItem.onVisibleChanged()

		FocusFrame {
		}
	}
	RowLayout {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		visible: popupTitle !== "" || popupText !== ""

		GButton {
			id: mailButton

			icon.source: "qrc:///images/email_icon.svg"
			//: LABEL DESKTOP
			text: qsTr("Send email")
			tintIcon: true
			visible: false

			onClicked: baseItem.emailButtonPressed()
		}
		GButton {
			icon.source: "qrc:/images/desktop/save_icon.svg"
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
			icon.source: "qrc:/images/desktop/info_white.svg"
			//: LABEL DESKTOP
			text: qsTr("See details")
			tintIcon: true
			visible: popupTitle !== "" || popupText !== ""

			onClicked: {
				let popup = detailedResultPopup.createObject(baseItem, {
						"text": popupText,
						"title": popupTitle
					});
				popup.open();
			}
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

		onClicked: baseItem.hintClicked()
	}
	GButton {
		id: button

		Layout.alignment: Qt.AlignHCenter
		Layout.preferredHeight: height
		Layout.preferredWidth: width
		activeFocusOnTab: true
		text: qsTr("OK")

		onClicked: baseItem.nextView(UiModule.DEFAULT)
	}
	Component {
		id: detailedResultPopup

		ConfirmationPopup {
			style: ConfirmationPopup.PopupStyle.OkButton
		}
	}
}
