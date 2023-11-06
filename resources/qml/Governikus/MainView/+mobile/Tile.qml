/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

AbstractButton {
	id: root

	readonly property bool flowVertically: height > topPadding + image.implicitHeight + layout.spacing + text.effectiveMaxLinesHeight + bottomPadding
	property alias image: image.source
	readonly property int insetBaseValue: Constants.component_spacing
	readonly property int paddingBaseValue: Constants.component_spacing
	property alias title: text.text

	Accessible.name: title
	Accessible.role: Accessible.Button
	Layout.maximumHeight: Number.POSITIVE_INFINITY
	Layout.maximumWidth: leftPadding + image.implicitWidth + layout.spacing + Math.ceil(text.implicitWidth) + rightPadding
	Layout.minimumHeight: topPadding + Math.max(image.implicitHeight, text.effectiveMaxLinesHeight) + bottomPadding
	Layout.minimumWidth: leftPadding + image.implicitWidth + rightPadding
	bottomInset: Math.ceil(insetBaseValue * 1.5)
	bottomPadding: bottomInset + paddingBaseValue
	implicitHeight: topPadding + layout.implicitHeight + bottomPadding
	implicitWidth: flowVertically ? 300 : Layout.maximumWidth
	leftInset: insetBaseValue
	leftPadding: leftInset + paddingBaseValue
	rightInset: insetBaseValue
	rightPadding: rightInset + paddingBaseValue
	topInset: insetBaseValue
	topPadding: topInset + paddingBaseValue

	background: GPaneBackground {
		Accessible.ignored: true
		color: root.pressed ? Style.color.control : Style.color.pane
		layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software

		layer.effect: GDropShadow {
		}

		Accessible.onPressAction: clicked()
	}
	contentItem: GridLayout {
		id: layout

		readonly property int spacing: Constants.component_spacing

		columnSpacing: spacing
		columns: 3
		flow: root.flowVertically ? GridLayout.TopToBottom : GridLayout.LeftToRight
		rowSpacing: spacing
		rows: 3

		TintableIcon {
			id: image

			Accessible.ignored: true
			sourceSize.height: Style.dimens.medium_icon_size
			tintColor: root.pressed ? Style.color.mainbutton_content_pressed : Style.color.text_subline
		}
		GText {
			id: text

			Accessible.ignored: true
			color: root.pressed ? Style.color.mainbutton_content_pressed : Style.color.text_title
			elide: Text.ElideRight
			horizontalAlignment: Text.AlignLeft
			maximumLineCount: 3
			rightPadding: root.flowVertically ? paddingBaseValue : 0
			textStyle: Style.text.title
		}
		GSpacer {
			Layout.fillHeight: true
			visible: root.flowVertically
		}
	}
}
