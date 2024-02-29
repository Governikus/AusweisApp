/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.View
import Governikus.Style
import Governikus.Type.ApplicationModel

ComboBox {
	id: control

	property TextStyle textStyle: Style.text.normal

	Accessible.name: displayText
	Accessible.role: Accessible.ComboBox
	font.pixelSize: textStyle.textSize
	popup.bottomMargin: plugin.safeAreaMargins.bottom
	popup.leftMargin: plugin.safeAreaMargins.left
	popup.rightMargin: plugin.safeAreaMargins.right
	popup.topMargin: plugin.safeAreaMargins.top
	spacing: Constants.groupbox_spacing

	background: GPaneBackground {
		border.color: control.textStyle.textColor
		color: Style.color.transparent
		drawShadow: false
	}
	contentItem: GText {
		elide: Text.ElideRight
		leftPadding: Constants.pane_padding
		maximumLineCount: 1
		padding: control.spacing
		rightPadding: control.spacing
		text: control.displayText
		textStyle: control.textStyle
	}
	delegate: ItemDelegate {
		highlighted: control.highlightedIndex === index
		width: control.width

		background: Rectangle {
			color: highlighted ? Style.color.control : Style.color.pane_sublevel
			implicitHeight: Style.dimens.list_item_height
			implicitWidth: 100

			GSeparator {
				anchors {
					bottom: parent.bottom
					left: parent.left
					right: parent.right
				}
			}
		}
		contentItem: GText {
			color: highlighted ? Style.color.control_content_hovered : control.textStyle.textColor
			elide: Text.ElideRight
			text: modelData
			textStyle: control.textStyle
		}
	}
	indicator: TintableIcon {
		height: control.height * 0.75
		source: down ? "qrc:///images/material_expand_less.svg" : "qrc:///images/material_expand_more.svg"
		tintColor: control.textStyle.textColor
		width: height
		x: Math.round(control.width - width - control.spacing)
		y: Math.round(control.topPadding + (control.availableHeight - height) / 2)
	}

	FocusFrame {
		marginFactor: 1.5
		scope: control
	}
}
