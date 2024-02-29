/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel

AbstractButton {
	id: root

	property alias buttonColor: d.color
	property alias cursorShape: mouseArea.cursorShape
	property string disabledTooltipText

	// Similar to "enabled", but tooltips will continue to work
	property bool enableButton: true
	property string enabledTooltipText
	property alias iconSize: buttonIcon.sourceSize.height
	property alias maximumLineCount: buttonText.maximumLineCount
	property color textDisabledColor: Style.color.control_content_disabled
	property color textHighlightColor: Style.color.control_content_pressed
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
	horizontalPadding: Constants.pane_padding
	verticalPadding: 6

	background: Rectangle {
		border.color: d.borderColor
		border.width: Style.dimens.border_width
		color: d.color
		radius: Style.dimens.control_radius

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: RowLayout {
		RowLayout {
			id: contentLayout

			Layout.maximumWidth: Number.POSITIVE_INFINITY
			Layout.minimumHeight: root.font.pixelSize + topPadding + bottomPadding
			Layout.minimumWidth: background ? Style.dimens.min_button_width - leftPadding - rightPadding : -1
			spacing: 0
			z: 1

			TintableIcon {
				id: buttonIcon

				Layout.rightMargin: Constants.groupbox_spacing
				source: root.icon.source
				sourceSize.height: 1.2 * buttonText.effectiveFirstLineHeight
				tintColor: d.contentColor
				tintEnabled: tintIcon
				visible: source != ""
				width: height
			}
			GText {
				id: buttonText

				Accessible.ignored: true
				Layout.alignment: Qt.AlignHCenter
				color: d.contentColor
				elide: Text.ElideRight
				font: root.font
				lineHeight: root.textStyle.lineHeight
				maximumLineCount: 1
				text: root.text
				visible: text !== ""
			}
			GSpacer {
				Layout.fillWidth: true
				visible: buttonIcon.visible
			}
		}
	}

	Accessible.onPressAction: clicked()
	onActiveFocusOnTabChanged: if (!activeFocusOnTab)
		focus = false

	Item {
		id: d

		property color borderColor: color === Style.color.control ? Style.color.control_border : color
		property color color: Style.color.control
		property color contentColor: root.textStyle === Style.text.button ? Style.color.control_content : root.textStyle.textColor

		states: [
			State {
				name: "disabled"
				when: !root.enabled || !root.enableButton

				PropertyChanges {
					d.borderColor: Style.color.control_border_disabled
					d.color: Style.color.control_disabled
					d.contentColor: root.textDisabledColor
				}
			},
			State {
				name: "pressed"
				when: root.pressed

				PropertyChanges {
					d.borderColor: Style.color.control_border_pressed
					d.color: Style.color.control_pressed
					d.contentColor: root.textHighlightColor
				}
			},
			State {
				name: "hovered"
				when: root.hovered

				PropertyChanges {
					d.borderColor: Style.color.control_border_hovered
					d.color: Style.color.control_hovered
					d.contentColor: root.textStyle === Style.text.button ? Style.color.control_content_hovered : root.textHighlightColor
				}
			},
			State {
				name: "unchecked"
				when: !root.checked && checkable

				PropertyChanges {
					d.borderColor: Style.color.control_border_unchecked
					d.color: Style.color.control_unchecked
					d.contentColor: Style.color.control_content_unchecked
				}
			}
		]
		transitions: [
			EaseInPressedTransition {
				target: d
			}
		]
	}
	FocusFrame {
		visible: !root.background
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
