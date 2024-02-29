/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel

Popup {
	id: root

	enum PopupStyle {
		NoButtons,
		OkButton,
		CancelButton
	}

	property alias buttons: buttonContainer.children
	//: LABEL ALL_PLATFORMS
	property string cancelButtonText: qsTr("Cancel")
	default property alias children: customContent.children
	property int horizontalTextAlignment: Text.AlignLeft
	//: LABEL ALL_PLATFORMS
	property string okButtonText: qsTr("OK")
	property int style: ConfirmationPopup.PopupStyle.OkButton | ConfirmationPopup.PopupStyle.CancelButton
	property string text
	property string title

	signal cancelled
	signal confirmed

	function accept() {
		root.confirmed();
		if (root.closePolicy !== Popup.NoAutoClose) {
			root.close();
		}
	}
	function cancel() {
		root.cancelled();
		if (root.closePolicy !== Popup.NoAutoClose) {
			root.close();
		}
	}

	anchors.centerIn: Overlay.overlay
	bottomMargin: Overlay.overlay ? 0.125 * Overlay.overlay.height : 0
	closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
	focus: true
	leftMargin: Overlay.overlay ? 0.125 * Overlay.overlay.width : 0
	modal: true
	padding: Constants.pane_padding / 2
	parent: Overlay.overlay
	rightMargin: leftMargin
	rightPadding: 0
	topMargin: bottomMargin

	background: Rectangle {
		border.color: Style.color.border
		border.width: Style.dimens.popup_border
		color: Style.color.background
		radius: Style.dimens.pane_radius
	}
	contentItem: GFlickableColumnLayout {
		bottomMargin: Constants.pane_padding / 2
		clip: true
		leftMargin: Constants.pane_padding / 2
		rightMargin: Constants.pane_padding
		spacing: Constants.component_spacing
		topMargin: Constants.pane_padding / 2
		width: availableWidth

		Keys.onEnterPressed: if (style & ConfirmationPopup.PopupStyle.OkButton)
			root.accept()
		Keys.onReturnPressed: if (style & ConfirmationPopup.PopupStyle.OkButton)
			root.accept()

		GText {
			activeFocusOnTab: true
			elide: Text.ElideRight
			focus: true
			horizontalAlignment: root.horizontalTextAlignment
			maximumLineCount: 5
			text: root.title
			textStyle: Style.text.headline
			visible: root.title !== ""

			FocusFrame {
			}
		}
		GText {
			activeFocusOnTab: true
			horizontalAlignment: root.horizontalTextAlignment
			text: root.text
			visible: root.text !== ""

			FocusFrame {
			}
		}
		Item {
			id: customContent

			Layout.fillWidth: true
			implicitHeight: childrenRect.height
			visible: children.length > 0
		}
		Item {
			id: buttonContainer

			Layout.fillWidth: true
			implicitHeight: childrenRect.height
			visible: style !== ConfirmationPopup.PopupStyle.NoButtons
		}
	}
}
