/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

AbstractButton {
	id: root

	property int borderWidth: Style.dimens.border_width
	property alias buttonColor: colors.controlBackground
	property alias cursorShape: mouseArea.cursorShape
	property string disabledTooltipText

	// Similar to "enabled", but tooltips will continue to work
	property bool enableButton: true
	property string enabledTooltipText
	property alias iconSize: buttonIcon.sourceSize.height
	property alias layoutDirection: contentLayout.layoutDirection
	property alias maximumLineCount: buttonText.maximumLineCount
	property real radius: Style.dimens.control_radius
	property alias style: colors.controlStyle
	property TextStyle textStyle: Style.text.button
	property bool tintIcon: false

	Accessible.name: text
	Layout.fillWidth: true
	Layout.maximumWidth: Math.ceil(implicitWidth)
	Layout.minimumWidth: background ? Style.dimens.min_button_width : -1
	ToolTip.delay: Constants.toolTipDelay
	ToolTip.text: enableButton ? enabledTooltipText : disabledTooltipText
	ToolTip.visible: hovered && ToolTip.text !== ""
	activeFocusOnTab: enableButton
	font.pixelSize: textStyle.textSize
	font.weight: textStyle.fontWeight
	horizontalPadding: Constants.control_horizontalPadding
	spacing: Constants.groupbox_spacing
	verticalPadding: Constants.control_verticalPadding

	background: Rectangle {
		border.color: colors.controlBorder
		border.width: root.borderWidth
		color: colors.controlBackground
		radius: root.radius
	}
	contentItem: RowLayout {
		RowLayout {
			id: contentLayout

			Layout.maximumWidth: Number.POSITIVE_INFINITY
			Layout.minimumHeight: root.font.pixelSize + topPadding + bottomPadding
			Layout.minimumWidth: background ? root.Layout.minimumWidth - leftPadding - rightPadding : -1
			spacing: root.spacing
			z: 1

			TintableIcon {
				id: buttonIcon

				source: root.icon.source
				sourceSize.height: 1.2 * buttonText.effectiveFirstLineHeight
				tintColor: colors.controlContent
				tintEnabled: tintIcon
				visible: source != ""
			}
			GText {
				id: buttonText

				Accessible.ignored: true
				Layout.alignment: Qt.AlignHCenter
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				color: colors.controlContent
				elide: Text.ElideRight
				font: root.font
				horizontalAlignment: {
					if (!buttonIcon.visible) {
						return Text.AlignHCenter;
					}
					return layoutDirection === Qt.LeftToRight ? Text.AlignLeft : Text.AlignRight;
				}
				lineHeight: root.textStyle.lineHeight
				maximumLineCount: 1
				text: root.text
				visible: text !== ""
			}
		}
	}

	onActiveFocusOnTabChanged: if (!activeFocusOnTab)
		focus = false

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		controlBorder: controlBackground === controlStyle.background.basic ? controlStyle.border.basic : controlBackground
		disabledCondition: !root.enableButton || !root.enabled
		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
	FocusFrame {
		marginFactor: root.background ? 0.8 : 1
		radius: root.background ? (1.2 * root.radius) : Math.min(height / 4, Style.dimens.control_radius)
		size: root.font.pixelSize / 8
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent
		z: 2

		onPressed: mouse => {
			mouse.accepted = !enableButton;
		}
	}
}