/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
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

	property alias buttonText: button.text
	property alias buttonType: button.buttonType
	property alias header: resultHeader.text
	property alias hintButtonText: hintItem.buttonText
	property alias hintText: hintItem.text
	property alias popupText: detailedResultPopup.text
	property alias popupTitle: detailedResultPopup.title
	property int resultType: ResultView.Type.IsSuccess
	property alias supportButtonsVisible: supportButtonsLayout.visible
	property alias text: resultText.text

	signal emailButtonPressed
	signal hintClicked

	Keys.onEnterPressed: button.onClicked()
	Keys.onEscapePressed: button.onClicked()
	Keys.onReturnPressed: button.onClicked()

	GFlickableColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		spacing: Constants.pane_spacing

		StatusIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredHeight: Style.dimens.status_icon_large
			Layout.preferredWidth: Style.dimens.status_icon_large
			source: {
				switch (resultType) {
				case ResultView.Type.IsSuccess:
					return "qrc:///images/status_ok.svg";
				case ResultView.Type.IsInfo:
					return "qrc:///images/status_info.svg";
				case ResultView.Type.IsError:
					return "qrc:///images/status_error.svg";
				}
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
		GText {
			id: resultHeader
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width
			activeFocusOnTab: true
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.header
			verticalAlignment: Text.AlignVCenter
			visible: text !== ""

			FocusFrame {
			}
		}
		GText {
			id: resultText
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width
			activeFocusOnTab: true
			horizontalAlignment: Text.AlignHCenter
			textStyle: resultHeader.visible ? Style.text.header_secondary : Style.text.header
			verticalAlignment: Text.AlignVCenter
			visible: text !== ""

			FocusFrame {
			}
		}
		RowLayout {
			id: supportButtonsLayout
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			spacing: Constants.component_spacing
			visible: false

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
				icon.source: "qrc:/images/info.svg"
				//: LABEL DESKTOP
				text: qsTr("See details")
				tintIcon: true
				visible: popupTitle !== "" || popupText !== ""

				onClicked: detailedResultPopup.open()
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
			visible: text !== ""

			onClicked: baseItem.hintClicked()
		}
		NavigationButton {
			id: button
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredHeight: height
			Layout.preferredWidth: width
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
