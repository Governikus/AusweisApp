/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View

GAbstractButton {
	id: root

	required property string desc
	property bool drawBottomCorners: false
	required property url img
	required property string name
	property alias tintIcon: icon.tintEnabled

	Accessible.description: desc
	Accessible.name: name
	Accessible.role: Accessible.RadioButton
	autoExclusive: true
	checkable: true
	horizontalPadding: Style.dimens.pane_spacing * 2
	icon.source: img
	text: name
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

			source: root.icon.source
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

	Accessible.onScrollDownAction: Utils.scrollPageDownOnGFlickable(this)
	Accessible.onScrollUpAction: Utils.scrollPageUpOnGFlickable(this)

	StatefulColors {
		id: colors

		checkedCondition: false
		paneStyle: Style.color.paneSublevel
		statefulControl: root
	}
	FocusFrame {
		marginFactor: -1
	}
}
