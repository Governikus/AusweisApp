/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Style
import Governikus.View

Switch {
	id: control

	property alias description: descriptionText.text
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	property alias overwriteHovered: colors.overwriteHovered
	property alias overwritePressed: colors.overwritePressed

	Accessible.name: text + ". " + description
	horizontalPadding: Constants.component_spacing
	indicator: null
	verticalPadding: Constants.component_spacing / 2

	background: RoundedRectangle {
		bottomLeftCorner: control.drawBottomCorners
		bottomRightCorner: control.drawBottomCorners
		color: colors.control_preferredPaneBackground
		topLeftCorner: control.drawTopCorners
		topRightCorner: control.drawTopCorners
	}
	contentItem: RowLayout {
		readonly property int focusWidth: layoutDirection === Qt.LeftToRight ? width : implicitWidth

		layoutDirection: Constants.is_desktop ? Qt.RightToLeft : Qt.LeftToRight
		spacing: Constants.component_spacing

		ColumnLayout {
			spacing: Constants.subtext_spacing
			visible: titleText.text !== "" || descriptionText.text !== ""

			GText {
				id: titleText

				Accessible.ignored: true
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				color: colors.text_subline
				text: control.text
				textStyle: Style.text.subline
				visible: text !== ""
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				color: colors.text
				visible: text !== ""
			}
		}
		Rectangle {
			Layout.alignment: Constants.is_desktop ? Qt.AlignTop : 0
			border.color: colors.control_border
			color: colors.control
			implicitHeight: implicitWidth / 2
			implicitWidth: Style.dimens.switch_width
			radius: height / 2

			Rectangle {
				id: ball

				readonly property int distanceBallBorder: 3

				anchors.verticalCenter: parent.verticalCenter
				color: colors.control_content
				height: parent.height - 2 * distanceBallBorder
				radius: height / 2
				width: height
				x: control.checked ? parent.width - width - distanceBallBorder : distanceBallBorder

				Behavior on x {
					NumberAnimation {
						duration: 200
						easing.type: Easing.InOutQuad
					}
				}
			}
		}
	}

	Accessible.onPressAction: toggle()

	StatefulColors {
		id: colors

		control_content: Style.color.control_content_graphics
		statefulControl: control
	}
	FocusFrame {
		anchors {
			bottomMargin: control.bottomPadding / 2
			leftMargin: control.leftPadding / 2
			rightMargin: Math.max(0, contentItem.width - contentItem.focusWidth) + control.rightPadding / 2
			topMargin: control.topPadding / 2
		}
	}
}
