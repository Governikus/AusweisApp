/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

GAbstractButton {
	id: root

	property alias colorStyle: colors.linkStyle
	property alias iconSize: linkIcon.sourceSize.height
	property alias iconTintColor: linkIcon.tintColor
	property alias layoutDirection: contentLayout.layoutDirection
	property alias maximumLineCount: linkText.maximumLineCount
	property alias textStyle: linkText.textStyle
	property alias tintIcon: linkIcon.tintEnabled

	Accessible.name: text
	Layout.maximumWidth: Math.ceil(implicitWidth)
	background: null
	baselineOffset: linkText.baselineOffset + verticalPadding + (contentItem.height - linkText.height) / 2
	font.pixelSize: linkText.textStyle.textSize
	font.underline: UiPluginModel.a11yButtonShapeActive
	font.weight: linkText.textStyle.fontWeight
	horizontalPadding: Style.dimens.control_horizontalPadding
	implicitWidth: linkText.visible ? (implicitContentWidth + leftPadding + rightPadding) : implicitHeight
	verticalPadding: Style.dimens.control_verticalPadding

	contentItem: RowLayout {
		RowLayout {
			id: contentLayout

			Layout.maximumWidth: Number.POSITIVE_INFINITY
			Layout.minimumHeight: root.font.pixelSize + root.topPadding + root.bottomPadding
			spacing: Style.dimens.groupbox_spacing
			z: 1

			TintableIcon {
				id: linkIcon

				Layout.alignment: Qt.AlignCenter
				source: root.icon.source
				sourceSize.height: 1.5 * linkText.effectiveFirstLineHeight
				tintColor: colors.linkColor
				visible: root.icon.source.toString() !== ""

				Behavior on source {
					enabled: SettingsModel.useAnimations && !Style.is_layout_desktop

					SequentialAnimation {
						PropertyAction {
							property: "opacity"
							target: linkIcon
							value: 0
						}
						PropertyAction {
							property: "source"
							target: linkIcon
						}
						PropertyAnimation {
							duration: Style.animation_duration * 2
							easing.type: Easing.InCubic
							property: "opacity"
							target: linkIcon
							to: 1
						}
					}
				}
			}
			GCrossBlendedText {
				id: linkText

				Accessible.ignored: true
				Layout.alignment: Qt.AlignHCenter
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				color: colors.linkColor
				elide: Text.ElideRight
				font: root.font
				horizontalAlignment: {
					if (!linkIcon.visible) {
						return Text.AlignHCenter;
					}
					return root.layoutDirection === Qt.LeftToRight ? Text.AlignLeft : Text.AlignRight;
				}
				lineHeight: root.textStyle.lineHeight
				maximumLineCount: 1
				text: root.text
				textStyle: Style.text.link
				visible: text !== ""
			}
		}
	}

	onActiveFocusOnTabChanged: if (!activeFocusOnTab)
		focus = false

	HoverHandler {
	}
	StatefulColors {
		id: colors

		statefulControl: root
	}
	FocusFrame {
	}
}
