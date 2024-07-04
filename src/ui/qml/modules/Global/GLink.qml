/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

AbstractButton {
	id: root

	property alias colorStyle: colors.linkStyle
	property alias cursorShape: mouseArea.cursorShape
	property alias iconSize: linkIcon.sourceSize.height
	property alias iconTintColor: linkIcon.tintColor
	property alias layoutDirection: contentLayout.layoutDirection
	property alias maximumLineCount: linkText.maximumLineCount
	property alias textStyle: linkText.textStyle
	property alias tintIcon: linkIcon.tintEnabled

	Accessible.name: text
	Layout.fillWidth: true
	Layout.maximumWidth: Math.ceil(implicitWidth)
	activeFocusOnTab: true
	background: null
	font.pixelSize: linkText.textStyle.textSize
	font.weight: linkText.textStyle.fontWeight
	horizontalPadding: Constants.control_horizontalPadding
	verticalPadding: Constants.control_verticalPadding

	contentItem: RowLayout {
		RowLayout {
			id: contentLayout

			Layout.maximumWidth: Number.POSITIVE_INFINITY
			Layout.minimumHeight: root.font.pixelSize + topPadding + bottomPadding
			spacing: Constants.groupbox_spacing
			z: 1

			TintableIcon {
				id: linkIcon

				source: root.icon.source
				sourceSize.height: 1.2 * linkText.effectiveFirstLineHeight
				tintColor: colors.linkColor
				tintEnabled: true
				visible: root.icon.source != ""

				Behavior on source {
					enabled: SettingsModel.useAnimations && !Constants.is_desktop

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
							duration: Constants.animation_duration * 2
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
					return layoutDirection === Qt.LeftToRight ? Text.AlignLeft : Text.AlignRight;
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
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		statefulControl: root
	}
	FocusFrame {
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent
		z: 2

		onPressed: mouse => {
			mouse.accepted = false;
		}
	}
}
