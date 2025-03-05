/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type

Item {
	id: root

	property alias color: mainText.color
	property alias effectiveFirstLineHeight: mainText.effectiveFirstLineHeight
	property alias elide: mainText.elide
	property alias font: mainText.font
	property alias horizontalAlignment: mainText.horizontalAlignment
	property alias lineHeight: mainText.lineHeight
	property alias maximumLineCount: mainText.maximumLineCount
	property alias text: mainText.text
	property alias textStyle: mainText.textStyle
	property alias wrapMode: mainText.wrapMode

	Accessible.ignored: text === ""
	Accessible.name: mainText.Accessible.name
	Accessible.role: mainText.Accessible.role
	Layout.fillWidth: true
	Layout.maximumWidth: Style.dimens.max_text_width
	implicitHeight: Math.max(mainText.implicitHeight, tempText.implicitHeight)
	implicitWidth: Math.max(mainText.implicitWidth, tempText.implicitWidth)

	Behavior on implicitHeight {
		enabled: SettingsModel.useAnimations && !Style.is_layout_desktop

		NumberAnimation {
			duration: mainText.text === "" ? 0 : Style.animation_duration
		}
	}

	GText {
		id: mainText

		Accessible.ignored: true
		activeFocusOnTab: false
		anchors.fill: parent

		Behavior on text {
			enabled: SettingsModel.useAnimations && !Style.is_layout_desktop

			SequentialAnimation {
				PropertyAction {
					property: "text"
					target: tempText
					value: mainText.text
				}
				PropertyAction {
					property: "text"
					target: mainText
				}
				PropertyAction {
					property: "opacity"
					target: mainText
					value: 0
				}
				PropertyAction {
					property: "opacity"
					target: tempText
					value: 1
				}
				PropertyAnimation {
					duration: mainText.text === "" ? 0 : Style.animation_duration
					easing.type: Easing.OutCubic
					property: "opacity"
					target: tempText
					to: 0
				}
				PropertyAnimation {
					duration: mainText.text === "" ? Style.animation_duration * 2 : Style.animation_duration
					easing.type: mainText.text === "" ? Easing.InOutCubic : Easing.InCubic
					property: "opacity"
					target: mainText
					to: 1
				}
				PropertyAction {
					property: "text"
					target: tempText
					value: ""
				}
			}
		}
	}
	GText {
		id: tempText

		Accessible.ignored: true
		activeFocusOnTab: false
		color: mainText.color
		elide: mainText.elide
		font: mainText.font
		horizontalAlignment: mainText.horizontalAlignment
		lineHeight: mainText.lineHeight
		maximumLineCount: mainText.maximumLineCount
		textStyle: mainText.textStyle
		wrapMode: mainText.wrapMode

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
}
