/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Popup {
	id: root

	enum PopupStyle {
		NoButtons = 0,
		OkButton = 1,
		CancelButton = 2
	}

	property alias buttons: buttonContainer.children
	default property alias children: customContent.children
	property string title
	property string text
	//: LABEL ALL_PLATFORMS
	property string okButtonText: qsTr("Ok") + SettingsModel.translationTrigger
	//: LABEL ALL_PLATFORMS
	property string cancelButtonText: qsTr("Cancel") + SettingsModel.translationTrigger
	property int style: ConfirmationPopup.PopupStyle.OkButton | ConfirmationPopup.PopupStyle.CancelButton
	property var horizontalTextAlignment: Text.AlignLeft

	signal confirmed
	signal cancelled

	function accept(){
		root.confirmed()
		if (root.closePolicy !== Popup.NoAutoClose) {
			root.close()
		}
	}

	function cancel() {
		root.cancelled()
		if (root.closePolicy !== Popup.NoAutoClose) {
			root.close()
		}
	}

	anchors.centerIn: Overlay.overlay
	margins: Constants.pane_padding
	padding: 0
	topPadding: Constants.pane_padding

	modal: true
	closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape

	background: Rectangle {
		color: Style.color.background_popup
		radius: Style.dimens.corner_radius_popup
	}

	ColumnLayout {
		id: contentLayout

		width: root.availableWidth
		spacing: Constants.pane_padding

		GText {
			visible: text !== ""
			Layout.fillWidth: true
			Layout.rightMargin: Constants.pane_padding
			Layout.leftMargin: Constants.pane_padding

			activeFocusOnTab: true

			text: root.title
			textStyle: Style.text.header
			font.bold: true
			horizontalAlignment: root.horizontalTextAlignment

			FocusFrame {
				dynamic: false
			}
		}

		GText {
			visible: text !== ""
			Layout.fillWidth: true
			Layout.rightMargin: Constants.pane_padding
			Layout.leftMargin: Constants.pane_padding

			activeFocusOnTab: true

			text: root.text
			textStyle: Style.text.normal
			horizontalAlignment: root.horizontalTextAlignment

			FocusFrame {
				dynamic: false
			}
		}

		Column {
			id: customContent
			visible: children.length > 0

			Layout.fillWidth: true
			Layout.rightMargin: Constants.pane_padding
			Layout.leftMargin: Constants.pane_padding
		}

		Item {
			id: buttonContainer

			Layout.fillWidth: true
			height: childrenRect.height
		}
	}
}

