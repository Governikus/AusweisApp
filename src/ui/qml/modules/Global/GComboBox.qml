/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Governikus.View
import Governikus.Style
import Governikus.Type

ComboBox {
	id: root

	property int radius: Style.dimens.pane_radius
	property TextStyle textStyle: Style.text.normal

	Accessible.name: displayText
	Accessible.role: Accessible.ComboBox
	font.pixelSize: textStyle.textSize
	horizontalPadding: Style.dimens.control_horizontalPadding
	leftPadding: horizontalPadding
	popup.bottomMargin: UiPluginModel.safeAreaMargins.bottom
	popup.leftMargin: UiPluginModel.safeAreaMargins.left
	popup.rightMargin: UiPluginModel.safeAreaMargins.right
	popup.topMargin: UiPluginModel.safeAreaMargins.top
	rightPadding: horizontalPadding
	spacing: Style.dimens.groupbox_spacing
	verticalPadding: Style.dimens.control_verticalPadding

	background: GPaneBackground {
		border.color: root.textStyle.textColor
		color: Style.color.transparent
		drawShadow: false
		radius: root.radius
	}
	contentItem: GText {
		Accessible.ignored: true
		activeFocusOnTab: false
		elide: Text.ElideRight
		font.pixelSize: root.font.pixelSize
		maximumLineCount: 1
		rightPadding: root.indicator.width + root.spacing
		text: root.displayText
		textStyle: root.textStyle
	}
	delegate: ItemDelegate {
		id: itemDelegate

		required property int index
		required property string modelData

		highlighted: root.highlightedIndex === index
		width: root.width

		background: Rectangle {
			color: itemDelegate.highlighted ? Style.color.control.background.basic : Style.color.paneSublevel.background.basic

			GSeparator {
				anchors {
					bottom: parent.bottom
					left: parent.left
					right: parent.right
				}
			}
		}
		contentItem: GText {
			activeFocusOnTab: false
			color: itemDelegate.highlighted ? Style.color.control.content.hovered : root.textStyle.textColor
			elide: Text.ElideRight
			font.pixelSize: root.font.pixelSize
			text: itemDelegate.modelData
			textStyle: root.textStyle
		}
	}
	indicator: Item {
		TintableIcon {
			source: root.down ? "qrc:///images/material_expand_less.svg" : "qrc:///images/material_expand_more.svg"
			sourceSize.height: root.font.pixelSize
			tintColor: root.textStyle.textColor
			x: Math.round(root.width - width - root.rightPadding)
			y: Math.round(root.topPadding + (root.availableHeight - height) / 2)
		}
	}

	FocusFrame {
		marginFactor: 1
		radius: 1.2 * root.radius
		size: root.font.pixelSize / 8
	}
}
