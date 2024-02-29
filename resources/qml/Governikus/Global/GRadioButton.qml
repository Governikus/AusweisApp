/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

RadioButton {
	id: root

	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	readonly property int indicatorHeight: Style.dimens.small_icon_size
	property alias overwriteHovered: colors.overwriteHovered
	property alias overwritePressed: colors.overwritePressed
	property var textStyle: Style.text.normal
	property bool tintIcon: false

	Accessible.name: text
	horizontalPadding: Constants.component_spacing
	indicator: null
	spacing: Constants.groupbox_spacing
	verticalPadding: Constants.component_spacing / 2

	background: RoundedRectangle {
		bottomLeftCorner: root.drawBottomCorners
		bottomRightCorner: root.drawBottomCorners
		color: colors.control_preferredPaneBackground
		topLeftCorner: root.drawTopCorners
		topRightCorner: root.drawTopCorners
	}
	contentItem: RowLayout {
		spacing: root.spacing

		Rectangle {
			border.color: colors.control_border
			border.width: Style.dimens.border_width
			color: colors.control
			implicitHeight: root.indicatorHeight
			implicitWidth: root.indicatorHeight
			radius: height / 2

			Rectangle {
				anchors.centerIn: parent
				color: colors.control_content
				height: parent.height / 2.5
				radius: height / 2
				visible: root.checked
				width: height
			}
		}
		TintableIcon {
			Layout.preferredHeight: sourceSize.height
			source: root.icon.source
			sourceSize.height: root.indicatorHeight
			tintColor: colors.text
			tintEnabled: tintIcon
			visible: source.toString() !== ""
		}
		GText {
			Accessible.ignored: true
			color: colors.text
			text: root.text
			textStyle: root.textStyle
			visible: text !== ""
		}
		GSpacer {
			Layout.fillWidth: true
			visible: root.text !== ""
		}
	}

	StatefulColors {
		id: colors

		control_content: Style.color.control_content_graphics
		hoveredCondition: (root.overwriteHovered || root.hovered) && Constants.is_desktop && !root.checked
		pressedCondition: (root.overwritePressed || root.pressed) && !root.checked
		statefulControl: root
	}
	FocusFrame {
		anchors {
			bottomMargin: root.bottomPadding / 2
			leftMargin: root.leftPadding / 2
			rightMargin: Math.max(0, contentItem.width - contentItem.implicitWidth) + root.rightPadding / 2
			topMargin: root.topPadding / 2
		}
	}
}
