/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Style
import Governikus.View

AbstractButton {
	id: root

	property alias description: descriptionText.text
	property bool isPane: false

	Accessible.name: root.text + ". " + root.description
	horizontalPadding: Style.dimens.pane_spacing
	verticalPadding: Style.dimens.pane_spacing

	background: GPaneBackground {
		id: pane

		border.color: colors.paneBorder
		color: colors.paneBackground

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: RowLayout {
		id: rowLayout

		spacing: Style.dimens.pane_spacing

		TintableIcon {
			source: root.icon.source
			sourceSize.height: Style.is_layout_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
			tintColor: title.color
		}
		ColumnLayout {
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			spacing: Style.dimens.text_spacing / 2

			GText {
				id: title

				Accessible.ignored: true
				activeFocusOnTab: false
				color: root.isPane ? colors.textSubline : colors.controlContent
				elide: Text.ElideRight
				text: root.text
				textStyle: Style.text.subline
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
				activeFocusOnTab: false
				color: root.isPane ? colors.textNormal : colors.controlContent
				elide: Text.ElideRight
				visible: text !== ""
			}
		}
		TintableIcon {
			source: "qrc:///images/material_arrow_right.svg"
			sourceSize.height: Style.is_layout_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
			tintColor: descriptionText.color
		}
	}

	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		hoveredCondition: hoverHandler.hovered
		paneStyle: root.isPane ? Style.color.pane : Style.color.control
		statefulControl: root
	}
}
