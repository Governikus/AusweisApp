/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

CheckBox {
	id: root

	property alias focusFrameVisible: focusFrame.visible
	property alias layoutDirection: contentLayout.layoutDirection
	property alias maximumLineCount: description.maximumLineCount
	readonly property color preferredBackgroundColor: colors.controlPreferredPaneBackground
	property alias textStyle: description.textStyle

	horizontalPadding: Style.dimens.pane_spacing
	indicator: null
	verticalPadding: Style.dimens.pane_spacing / 2

	contentItem: RowLayout {
		id: contentLayout

		readonly property int focusWidth: layoutDirection === Qt.RightToLeft ? width : implicitWidth

		spacing: root.spacing

		Rectangle {
			border.color: colors.controlBorder
			border.width: Math.max(checkmark.implicitHeight / 16, 1)
			color: colors.controlBackground
			implicitHeight: checkmark.implicitHeight + 4 * border.width
			implicitWidth: implicitHeight
			radius: Math.max(height / 6, 1)

			TintableIcon {
				id: checkmark

				anchors.centerIn: parent
				source: "qrc:///images/checkbox_indicator.svg"
				sourceSize.height: 1.2 * description.font.pixelSize
				tintColor: colors.controlContent
				visible: root.checked
			}
		}
		GText {
			id: description

			Accessible.ignored: true
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			elide: Text.ElideRight
			maximumLineCount: 1
			text: root.text
			visible: text !== ""
		}
	}

	Accessible.onPressAction: {
		toggle();
		toggled();
	}
	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		controlStyle: Style.color.controlCheckbox
		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
	FocusFrame {
		id: focusFrame

		size: root.textStyle.textSize / 8
	}
}
