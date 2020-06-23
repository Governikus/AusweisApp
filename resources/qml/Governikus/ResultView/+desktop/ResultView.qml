/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: baseItem

	enum Type {
		IsSuccess,
		IsError,
		IsInfo
	}

	signal emailButtonPressed()
	property alias supportButtonsVisible: supportButtonsLayout.visible
	property alias text: resultText.text
	property alias header: resultHeader.text
	property int resultType: ResultView.Type.IsSuccess
	property alias buttonType: button.buttonType
	property alias popupText: detailedResultPopup.text
	property alias popupTitle: detailedResultPopup.title

	Accessible.name: qsTr("Result view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the result of an authentication.") + SettingsModel.translationTrigger
	Keys.onReturnPressed: button.onClicked()
	Keys.onEnterPressed: button.onClicked()
	Keys.onEscapePressed: button.onClicked()

	ColumnLayout {
		anchors.fill: parent

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

			GButton {
				icon.source: "qrc:///images/provider/mail.svg"
				//: LABEL DESKTOP_QML
				text: qsTr("Send email") + SettingsModel.translationTrigger
				onClicked: baseItem.emailButtonPressed()
			}

			GButton {
				icon.source: "qrc:/images/desktop/info_application_log.svg"
				//: LABEL DESKTOP_QML
				text: qsTr("Save logfile") + SettingsModel.translationTrigger
				onClicked: {
					LogModel.setLogfile(0)
					let filenameSuggestion = LogModel.createLogFileName(LogModel.getCurrentLogfileDate())
					appWindow.openSaveFileDialog(LogModel.saveCurrentLogfile, filenameSuggestion, qsTr("Logfiles"), "log")
				}
			}

			GButton {
				visible: popupTitle !== "" || popupText !== ""

				icon.source: "qrc:/images/desktop/main_info.svg"
				//: LABEL DESKTOP_QML
				text: qsTr("See details") + SettingsModel.translationTrigger
				onClicked: detailedResultPopup.open()
			}
		}


		NavigationButton {
			id: button

			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: width
			Layout.preferredHeight: height

			activeFocusOnTab: true

			buttonType: NavigationButton.Type.Forward
			onClicked: baseItem.nextView(SectionPage.Views.Main)
		}
	}

	ConfirmationPopup {
		id: detailedResultPopup

		style: ConfirmationPopup.PopupStyle.OkButton
	}
}
