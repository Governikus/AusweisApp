/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View

AbstractButton {
	id: root

	property bool drawBottomCorners: false
	required property url image
	property alias tintIcon: icon.tintEnabled

	horizontalPadding: Style.dimens.pane_spacing * 2
	verticalPadding: Style.dimens.pane_spacing / 2

	background: RoundedRectangle {
		bottomLeftCorner: root.drawBottomCorners
		bottomRightCorner: root.drawBottomCorners
		color: colors.paneBackground
		topLeftCorner: false
		topRightCorner: false
	}
	contentItem: RowLayout {
		spacing: Style.dimens.pane_spacing

		TintableIcon {
			id: icon

			source: root.image
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: label.color
			tintEnabled: false
		}
		GText {
			id: label

			Accessible.ignored: true
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			color: colors.textNormal
			text: root.text
		}
	}

	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	StatefulColors {
		id: colors

		paneStyle: Style.color.paneSublevel
		statefulControl: root
	}
	FocusFrame {
		marginFactor: -1
	}
}
