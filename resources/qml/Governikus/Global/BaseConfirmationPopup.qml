/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

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
	property TextStyle headerTextStyle
	property var horizontalTextAlignment: Text.AlignLeft
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
	closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
	contentWidth: Overlay.overlay ? Math.min(0.75 * Overlay.overlay.width, Style.dimens.max_text_width) : 0
	focus: true
	margins: Constants.pane_padding
	modal: true
	padding: 0
	parent: Overlay.overlay
	topPadding: Constants.pane_padding

	background: Rectangle {
		border.color: Style.color.border
		border.width: Style.dimens.popup_border
		color: Style.color.background_popup
		radius: Style.dimens.corner_radius_popup
	}

	Item {
		id: contentItem
		implicitHeight: contentLayout.height
		implicitWidth: root.availableWidth

		Keys.onEnterPressed: if (style & ConfirmationPopup.PopupStyle.OkButton)
			root.accept()
		Keys.onReturnPressed: if (style & ConfirmationPopup.PopupStyle.OkButton)
			root.accept()

		ColumnLayout {
			id: contentLayout
			spacing: Constants.pane_padding
			width: parent.width

			GText {
				Layout.fillWidth: true
				Layout.leftMargin: Constants.pane_padding
				Layout.rightMargin: Constants.pane_padding
				activeFocusOnTab: true
				elide: Text.ElideRight
				focus: true
				font.bold: true
				horizontalAlignment: root.horizontalTextAlignment
				maximumLineCount: 5
				text: root.title
				textStyle: root.headerTextStyle
				visible: text !== ""

				FocusFrame {
				}
			}
			Item {
				Layout.fillWidth: true
				implicitHeight: infoTextFlickable.implicitHeight
				visible: infoText.text !== ""

				GFlickable {
					id: infoTextFlickable
					anchors.fill: parent
					anchors.leftMargin: Constants.pane_padding
					anchors.rightMargin: Constants.pane_padding
					clip: true
					contentHeight: infoText.implicitHeight
					contentWidth: width
					implicitHeight: root.Overlay.overlay ? Math.min(infoText.height, 0.5 * root.Overlay.overlay.height) : 0

					GText {
						id: infoText
						activeFocusOnTab: true
						height: implicitHeight
						horizontalAlignment: root.horizontalTextAlignment
						rightPadding: Constants.pane_padding
						text: root.text
						width: parent.width
					}
				}
				FocusFrame {
					framee: infoTextFlickable
					scope: infoText
				}
			}
			Column {
				id: customContent
				Layout.fillWidth: true
				Layout.leftMargin: Constants.pane_padding
				Layout.rightMargin: Constants.pane_padding
				visible: children.length > 0
			}
			Item {
				id: buttonContainer
				Layout.fillWidth: true
				Layout.preferredHeight: childrenRect.height
			}
		}
	}
}
