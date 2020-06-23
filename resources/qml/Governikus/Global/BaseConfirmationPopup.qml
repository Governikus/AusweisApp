/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
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
	property TextStyle headerTextStyle
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
	contentWidth: Overlay.overlay ? Math.min(0.75 * Overlay.overlay.width, Style.dimens.max_text_width) : 0
	margins: Constants.pane_padding
	padding: 0
	topPadding: Constants.pane_padding

	modal: true
	closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
	focus: true

	background: Rectangle {
		color: Style.color.background_popup
		radius: Style.dimens.corner_radius_popup
		border.color: Style.color.border
		border.width: Style.dimens.popup_border

		FocusPoint {
			anchors.margins: 4 * ApplicationModel.scaleFactor

			scope: contentItem
		}
	}

	Item {
		id: contentItem

		implicitWidth: root.availableWidth
		implicitHeight: contentLayout.height

		focus: true
		activeFocusOnTab: true
		Accessible.role: Accessible.Grouping
		Accessible.name: qsTr("Dialog") + SettingsModel.translationTrigger
		Keys.onEnterPressed: if (style & ConfirmationPopup.PopupStyle.OkButton) root.accept()
		Keys.onReturnPressed: if (style & ConfirmationPopup.PopupStyle.OkButton) root.accept()

		ColumnLayout {
			id: contentLayout

			width: parent.width
			spacing: Constants.pane_padding

			GText {
				visible: text !== ""
				Layout.fillWidth: true
				Layout.rightMargin: Constants.pane_padding
				Layout.leftMargin: Constants.pane_padding

				activeFocusOnTab: true

				text: root.title
				textStyle: root.headerTextStyle
				font.bold: true
				horizontalAlignment: root.horizontalTextAlignment

				FocusFrame {}
			}

			GText {
				visible: text !== ""
				Layout.fillWidth: true
				Layout.rightMargin: Constants.pane_padding
				Layout.leftMargin: Constants.pane_padding

				activeFocusOnTab: true

				text: root.text
				horizontalAlignment: root.horizontalTextAlignment

				FocusFrame {}
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
}
