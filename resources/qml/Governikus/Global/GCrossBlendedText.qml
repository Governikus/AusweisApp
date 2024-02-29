/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

Item {
	id: root

	property alias color: mainText.color
	property alias elide: mainText.elide
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

	GText {
		id: mainText

		Accessible.ignored: true
		anchors.fill: parent

		Behavior on text {
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
					duration: mainText.text === "" ? 0 : Constants.animation_duration
					easing.type: Easing.OutCubic
					property: "opacity"
					target: tempText
					to: 0
				}
				PropertyAnimation {
					duration: mainText.text === "" ? Constants.animation_duration * 2 : Constants.animation_duration
					easing.type: mainText.text === "" ? Easing.InOutCubic : Easing.InCubic
					property: "opacity"
					target: mainText
					to: 1
				}
			}
		}
	}
	GText {
		id: tempText

		Accessible.ignored: true
		color: mainText.color
		elide: mainText.elide
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
