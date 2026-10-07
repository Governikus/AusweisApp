/**
 * Copyright (c) 2023-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

ColumnLayout {
	id: root

	property string a11yDescription: root.description
	property bool alwaysReserveDescriptionHeight: false
	property bool arrowToLeft: false
	property alias backgroundColor: collapsibleContentBackground.color
	readonly property alias content: contentItem.children
	property int contentBottomMargin: Style.dimens.groupbox_spacing
	property int contentHorizontalMargin: horizontalMargin
	property alias contentSpacing: contentItem.spacing
	property int contentTopMargin: Style.dimens.groupbox_spacing
	property alias description: description.text
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	default property alias expandableData: contentItem.data
	property alias expanded: expandButton.expanded
	property int horizontalMargin: Style.dimens.pane_spacing
	property alias icon: icon.source
	property bool startExpanded: false
	property alias tintIcon: icon.tintEnabled
	property alias title: title.text

	spacing: 0

	GAbstractButton {
		id: expandButton

		property bool expanded: false

		Accessible.description: Utils.resolveA11yDescription(root.title, root.a11yDescription)
		Accessible.expandable: true
		Accessible.expanded: expanded
		Accessible.name: Utils.resolveA11yName(root.title, root.a11yDescription)
		implicitHeight: bannerLayout.implicitHeight + Style.dimens.pane_spacing * 2
		implicitWidth: bannerLayout.implicitWidth

		background: RoundedRectangle {
			id: background

			bottomLeftCorner: root.drawBottomCorners && !expandButton.expanded
			bottomRightCorner: root.drawBottomCorners && !expandButton.expanded
			color: colors.paneBackground
			topLeftCorner: root.drawTopCorners
			topRightCorner: root.drawTopCorners

			FocusFrame {
				anchors.margins: Style.dimens.pane_spacing / 2
				scope: expandButton
			}
		}
		contentItem: RowLayout {
			id: bannerLayout

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			spacing: 0

			LeftRightArrow {
				visible: root.arrowToLeft
			}
			ColumnLayout {
				Layout.leftMargin: root.arrowToLeft ? 0 : root.horizontalMargin
				Layout.rightMargin: root.arrowToLeft ? root.horizontalMargin : 0
				spacing: Style.dimens.subtext_spacing

				GText {
					id: title

					Accessible.ignored: true
					textStyle: Style.text.subline
					visible: text !== ""
				}
				GText {
					id: description

					Accessible.ignored: true
					visible: root.alwaysReserveDescriptionHeight || text !== ""

					Behavior on text {
						SequentialAnimation {
							PropertyAnimation {
								duration: Style.animation_duration
								easing.type: Easing.InCubic
								property: "opacity"
								target: description
								to: 0
							}
							PropertyAction {
								property: "text"
								target: description
							}
							PropertyAnimation {
								duration: Style.animation_duration
								easing.type: Easing.OutCubic
								property: "opacity"
								target: description
								to: 1
							}
						}
					}
				}
			}
			GSpacer {
				Layout.fillWidth: true
			}
			TintableIcon {
				id: icon

				Layout.rightMargin: root.arrowToLeft ? root.horizontalMargin : 0
				sourceSize.height: Style.dimens.small_icon_size
				tintColor: Style.color.textNormal.basic_unchecked
				tintEnabled: false
				visible: source.toString() !== ""
			}
			LeftRightArrow {
				visible: !root.arrowToLeft
			}
		}

		Accessible.onScrollDownAction: Utils.scrollPageDownOnGFlickable(this)
		Accessible.onScrollUpAction: Utils.scrollPageUpOnGFlickable(this)
		Component.onCompleted: expanded = root.startExpanded
		onClicked: expanded = !expanded

		StatefulColors {
			id: colors

			checkedCondition: false
			statefulControl: expandButton
		}
	}
	RoundedRectangle {
		id: collapsibleContentBackground

		Layout.fillWidth: true
		bottomLeftCorner: root.drawBottomCorners
		bottomRightCorner: root.drawBottomCorners
		clip: true
		color: Style.color.paneSublevel.background.basic_unchecked
		implicitHeight: expandButton.expanded ? (contentItem.implicitHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin) : 0
		implicitWidth: contentItem.implicitWidth + contentItem.anchors.leftMargin + contentItem.anchors.rightMargin
		topLeftCorner: false
		topRightCorner: false
		visible: expandButton.expanded

		Behavior on implicitHeight {
			NumberAnimation {
				duration: Style.animation_duration
			}
		}

		ColumnLayout {
			id: contentItem

			spacing: Style.dimens.groupbox_spacing

			anchors {
				bottomMargin: root.contentBottomMargin
				fill: parent
				leftMargin: root.contentHorizontalMargin
				rightMargin: root.contentHorizontalMargin
				topMargin: root.contentTopMargin
			}
		}
	}

	component LeftRightArrow: TintableIcon {
		Layout.leftMargin: root.horizontalMargin
		Layout.rightMargin: root.horizontalMargin
		source: expandButton.expanded ? "qrc:///images/material_expand_less.svg" : "qrc:///images/material_expand_more.svg"
		sourceSize.height: Style.text.normal.textSize
		tintColor: Style.color.textNormal.basic_unchecked
		tintEnabled: true
	}
}
