/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
	Layout.minimumWidth: Style.dimens.min_button_width
	ToolTip.delay: Constants.toolTipDelay
	ToolTip.text: enableButton ? enabledTooltipText : disabledTooltipText
	ToolTip.visible: hovered && ToolTip.text !== ""
	activeFocusOnTab: enableButton
	font.pixelSize: textStyle.textSize
	horizontalPadding: 8
	verticalPadding: 6

	background: Rectangle {
		border.color: d.borderColor
		border.width: Style.dimens.border_width
		color: d.color
		radius: Style.dimens.control_radius

		FocusFrame {
			borderColor: Style.color.control_border
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
			Layout.minimumWidth: Style.dimens.min_button_width - leftPadding - rightPadding
			spacing: 0
			z: 1

			TintableIcon {
				id: buttonIcon

				Layout.rightMargin: Constants.text_spacing
				source: root.icon.source
				sourceSize.height: contentLayout.Layout.minimumHeight
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

				FocusFrame {
					marginFactor: 0.7
					scope: root
					visible: !root.background
				}
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
					borderColor: Style.color.control_border_disabled
					color: Style.color.control_disabled
					contentColor: root.textDisabledColor
					target: d
				}
			},
			State {
				name: "pressed"
				when: root.pressed

				PropertyChanges {
					borderColor: Style.color.control_border_pressed
					color: Style.color.control_pressed
					contentColor: root.textHighlightColor
					target: d
				}
			},
			State {
				name: "hover"
				when: root.hovered

				PropertyChanges {
					borderColor: Style.color.control_border_hover
					color: Style.color.control_hover
					contentColor: root.textStyle === Style.text.button ? Style.color.control_content_hover : root.textHighlightColor
					target: d
				}
			},
			State {
				name: "unchecked"
				when: !root.checked && checkable

				PropertyChanges {
					borderColor: Style.color.control_border_unchecked
					color: Style.color.control_unchecked
					contentColor: Style.color.control_content_unchecked
					target: d
				}
			}
		]
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
