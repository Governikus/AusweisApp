/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

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
	property alias mainTextFormat: mainText.textFormat
	//: LABEL ALL_PLATFORMS
	property string okButtonText: qsTr("OK")
	property alias showCloseButton: closeButton.visible
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

	anchors.centerIn: parent
	bottomMargin: parent ? 0.125 * parent.height : 0
	closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
	focus: true
	leftMargin: parent ? 0.125 * parent.width : 0
	modal: true
	padding: Style.dimens.pane_padding / 2
	// missing-property is disabled because of QTBUG-13112
	parent: Overlay.overlay // qmllint disable missing-property
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
		bottomMargin: Style.dimens.pane_padding / 2
		clip: true
		leftMargin: Style.dimens.pane_padding / 2
		rightMargin: Style.dimens.pane_padding
		spacing: Style.dimens.pane_spacing
		topMargin: Style.dimens.pane_padding / 2
		width: root.availableWidth

		Keys.onEnterPressed: if (root.style & ConfirmationPopup.PopupStyle.OkButton)
			root.accept()
		Keys.onReturnPressed: if (root.style & ConfirmationPopup.PopupStyle.OkButton)
			root.accept()

		RowLayout {
			GText {
				elide: Text.ElideRight
				focus: true
				horizontalAlignment: root.horizontalTextAlignment
				maximumLineCount: 5
				text: root.title
				textStyle: Style.text.headline
				visible: root.title !== ""
			}
			GSpacer {
				Layout.fillWidth: true
			}
			GLink {
				id: closeButton

				colorStyle: Style.color.linkTitle
				horizontalPadding: 0
				icon.source: "qrc:///images/material_close.svg"
				iconSize: Style.dimens.small_icon_size
				tintIcon: true
				verticalPadding: 0
				visible: false

				onClicked: root.close()
			}
		}
		GText {
			id: mainText

			Layout.maximumWidth: Number.POSITIVE_INFINITY
			Layout.preferredWidth: Math.ceil(hiddenText.implicitWidth)
			horizontalAlignment: root.horizontalTextAlignment
			text: root.text
			textFormat: Text.RichText
			visible: root.text !== ""

			GText {
				id: hiddenText

				text: root.text
				visible: false
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
			visible: root.style !== ConfirmationPopup.PopupStyle.NoButtons
		}
	}
}
