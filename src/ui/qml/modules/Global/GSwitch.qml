/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Style
import Governikus.View

Switch {
	id: root

	property alias description: descriptionText.text
	property bool drawBottomCorners: false
	property bool drawTopCorners: false

	Accessible.name: text + ". " + description
	horizontalPadding: Style.dimens.pane_spacing
	indicator: null
	verticalPadding: Style.dimens.pane_spacing

	background: RoundedRectangle {
		bottomLeftCorner: root.drawBottomCorners
		bottomRightCorner: root.drawBottomCorners
		color: colors.controlPreferredPaneBackground
		topLeftCorner: root.drawTopCorners
		topRightCorner: root.drawTopCorners
	}
	contentItem: RowLayout {
		id: switchContent

		readonly property int focusWidth: layoutDirection === Qt.LeftToRight ? width : implicitWidth

		layoutDirection: Style.is_layout_desktop ? Qt.RightToLeft : Qt.LeftToRight
		spacing: Style.dimens.pane_spacing

		ColumnLayout {
			spacing: Style.dimens.subtext_spacing
			visible: titleText.text !== "" || descriptionText.text !== ""

			GText {
				id: titleText

				Accessible.ignored: true
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				activeFocusOnTab: false
				text: root.text
				textStyle: Style.text.subline
				visible: text !== ""
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				activeFocusOnTab: false
				visible: text !== ""
			}
		}
		Rectangle {
			border.color: colors.controlBorder
			color: colors.controlBackground
			implicitHeight: implicitWidth / 2
			implicitWidth: Style.dimens.switch_width
			radius: height / 2

			Rectangle {
				id: ball

				readonly property int distanceBallBorder: 3

				anchors.verticalCenter: parent.verticalCenter
				color: colors.controlContent
				height: parent.height - 2 * distanceBallBorder
				radius: height / 2
				width: height
				x: root.checked ? parent.width - width - distanceBallBorder : distanceBallBorder

				Behavior on x {
					enabled: hoverHandler.hovered

					NumberAnimation {
						duration: 200
						easing.type: Easing.InOutQuad
					}
				}
			}
		}
	}

	Accessible.onPressAction: toggle()
	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		controlStyle: Style.color.controlSwitch
		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
	FocusFrame {
		anchors {
			bottomMargin: root.bottomPadding / 2
			leftMargin: root.leftPadding / 2
			rightMargin: Math.max(0, switchContent.width - switchContent.focusWidth) + root.rightPadding / 2
			topMargin: root.topPadding / 2
		}
	}
}
