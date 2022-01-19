/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: baseItem

	enum Type {
		IsSuccess,
		IsError,
		IsInfo
	}

	property alias supportButtonsVisible: supportButtonsLayout.visible
	property alias text: resultText.text
	property alias header: resultHeader.text
	property alias buttonType: button.buttonType
	property alias popupText: detailedResultPopup.text
	property alias popupTitle: detailedResultPopup.title
	property alias hintText: hintItem.text
	property alias hintButtonText: hintItem.buttonText
	property int resultType: ResultView.Type.IsSuccess
	signal emailButtonPressed()
	signal hintClicked()

	Accessible.name: qsTr("Result view")
	Accessible.description: qsTr("This is the result of an authentication.")
	Keys.onReturnPressed: button.onClicked()
	Keys.onEnterPressed: button.onClicked()
	Keys.onEscapePressed: button.onClicked()

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		spacing: Constants.pane_spacing

		StatusIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: Style.dimens.status_icon_large
			Layout.preferredHeight: Style.dimens.status_icon_large

			source: {
				switch (resultType) {
					case ResultView.Type.IsSuccess:
						return "qrc:///images/status_ok.svg"
					case ResultView.Type.IsInfo:
						return "qrc:///images/status_info.svg"
					case ResultView.Type.IsError:
						return "qrc:///images/status_error.svg"
				}
			}
		}

		GSpacer { Layout.fillHeight: true }

		GText {
			id: resultHeader

			visible: text !== ""
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width

			activeFocusOnTab: true

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			textStyle: Style.text.header_inverse

			FocusFrame {}
		}

		GText {
			id: resultText

			visible: text !== ""
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width

			activeFocusOnTab: true

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			textStyle: resultHeader.visible ? Style.text.header_secondary_inverse : Style.text.header_inverse

			FocusFrame {}
		}

		RowLayout {
			id: supportButtonsLayout

			visible: false
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			spacing: Constants.component_spacing

			GButton {
				icon.source: "qrc:///images/material_mail.svg"
				//: LABEL DESKTOP
				text: qsTr("Send email")
				tintIcon: true
				onClicked: baseItem.emailButtonPressed()
			}

			GButton {
				icon.source: "qrc:/images/desktop/material_save.svg"
				//: LABEL DESKTOP
				text: qsTr("Save log")
				tintIcon: true
				onClicked: {
					LogModel.setLogFile(0)
					let filenameSuggestion = LogModel.createLogFileName(LogModel.getCurrentLogFileDate())
					appWindow.openSaveFileDialog(LogModel.saveCurrentLogFile, filenameSuggestion, qsTr("Logfiles"), "log")
				}
			}

			GButton {
				visible: popupTitle !== "" || popupText !== ""

				icon.source: "qrc:/images/info.svg"
				//: LABEL DESKTOP
				text: qsTr("See details")
				tintIcon: true
				onClicked: detailedResultPopup.open()
			}
		}

		GSpacer { Layout.fillHeight: true }

		Hint {
			id: hintItem

			visible: text !== ""

			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width

			onClicked: baseItem.hintClicked()
		}

		NavigationButton {
			id: button

			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: width
			Layout.preferredHeight: height

			activeFocusOnTab: true

			buttonType: NavigationButton.Type.Forward
			onClicked: baseItem.nextView(UiModule.DEFAULT)
		}
	}

	ConfirmationPopup {
		id: detailedResultPopup

		style: ConfirmationPopup.PopupStyle.OkButton
	}
}
