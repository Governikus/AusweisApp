/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.View
import Governikus.Style
import Governikus.Type

ComboBox {
	id: control

	property int radius: Style.dimens.pane_radius
	property TextStyle textStyle: Style.text.normal

	Accessible.name: displayText
	Accessible.role: Accessible.ComboBox
	font.pixelSize: textStyle.textSize
	horizontalPadding: Constants.control_horizontalPadding
	leftPadding: horizontalPadding
	popup.bottomMargin: UiPluginModel.safeAreaMargins.bottom
	popup.leftMargin: UiPluginModel.safeAreaMargins.left
	popup.rightMargin: UiPluginModel.safeAreaMargins.right
	popup.topMargin: UiPluginModel.safeAreaMargins.top
	rightPadding: horizontalPadding
	spacing: Constants.groupbox_spacing
	verticalPadding: Constants.control_verticalPadding

	background: GPaneBackground {
		border.color: control.textStyle.textColor
		color: Style.color.transparent
		drawShadow: false
		radius: control.radius
	}
	contentItem: GText {
		elide: Text.ElideRight
		font.pixelSize: control.font.pixelSize
		maximumLineCount: 1
		rightPadding: control.indicator.width + control.spacing
		text: control.displayText
		textStyle: control.textStyle
	}
	delegate: ItemDelegate {
		highlighted: control.highlightedIndex === index
		width: control.width

		background: Rectangle {
			color: highlighted ? Style.color.control.background.basic : Style.color.paneSublevel.background.basic

			GSeparator {
				anchors {
					bottom: parent.bottom
					left: parent.left
					right: parent.right
				}
			}
		}
		contentItem: GText {
			color: highlighted ? Style.color.control.content.hovered : control.textStyle.textColor
			elide: Text.ElideRight
			font.pixelSize: control.font.pixelSize
			text: modelData
			textStyle: control.textStyle
		}
	}
	indicator: Item {
		TintableIcon {
			source: down ? "qrc:///images/material_expand_less.svg" : "qrc:///images/material_expand_more.svg"
			sourceSize.height: control.font.pixelSize
			tintColor: control.textStyle.textColor
			x: Math.round(control.width - width - control.rightPadding)
			y: Math.round(control.topPadding + (control.availableHeight - height) / 2)
		}
	}

	FocusFrame {
		marginFactor: 1
		radius: 1.2 * control.radius
		size: control.font.pixelSize / 8
	}
}
