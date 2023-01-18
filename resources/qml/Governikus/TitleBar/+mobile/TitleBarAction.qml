/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property alias icon: imageItem.source
	readonly property real minimumWidth: d.isEmpty ? 0 : Math.max(implicitHeight, d.textWidth)
	property alias text: textItem.text

	signal clicked

	Accessible.focusable: true
	Accessible.ignored: d.isEmpty
	Accessible.name: text
	Accessible.role: Accessible.Button
	height: implicitHeight
	implicitHeight: Style.dimens.titlebar_height
	implicitWidth: d.isEmpty ? 0 : Math.max(implicitHeight, d.imageWidth + d.textWidth)
	width: implicitWidth

	Accessible.onPressAction: clicked()

	QtObject {
		id: d

		readonly property int imageWidth: imageItem.visible ? imageItem.x + imageItem.width : 0
		readonly property bool isEmpty: imageItem.source == "" && textItem.text === ""
		readonly property int textWidth: textItem.anchors.leftMargin + textItem.implicitWidth
	}
	TintableIcon {
		id: imageItem
		height: width
		playAnimation: true
		sourceSize.width: width
		tintColor: Style.color.primary_text_inverse
		visible: imageItem.source != ""
		width: Style.dimens.small_icon_size

		anchors {
			horizontalCenter: baseItem.left
			horizontalCenterOffset: baseItem.implicitHeight / 2
			verticalCenter: parent.verticalCenter
		}
	}
	GText {
		id: textItem
		Accessible.ignored: true
		elide: Text.ElideRight
		maximumLineCount: 1
		textStyle: Style.text.normal_inverse
		verticalAlignment: Text.AlignVCenter
		visible: !imageItem.visible || baseItem.width >= baseItem.implicitWidth
		wrapMode: Text.NoWrap

		Behavior on text  {
			SequentialAnimation {
				PropertyAnimation {
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
					property: "opacity"
					target: textItem
					to: 0
				}
				PropertyAction {
					property: "text"
					target: textItem
				}
				PropertyAnimation {
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
					property: "opacity"
					target: textItem
					to: 1
				}
			}
		}

		onVisibleChanged: {
			if (visible) {
				fadeIn.start();
			} else {
				fadeOut.start();
			}
		}

		anchors {
			left: imageItem.visible ? imageItem.right : parent.left
			leftMargin: imageItem.visible || text === "" ? 0 : Style.dimens.titlebar_padding
			verticalCenter: parent.verticalCenter
		}
		PropertyAnimation {
			id: fadeOut
			duration: Constants.animation_duration
			easing.type: Easing.InCubic
			property: "opacity"
			target: textItem
			to: 0
		}
		PropertyAnimation {
			id: fadeIn
			duration: Constants.animation_duration
			easing.type: Easing.OutCubic
			property: "opacity"
			target: textItem
			to: 1
		}
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
}
