/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

ColumnLayout {
	id: root

	property bool alwaysReserveSelectionTitleHeight: false
	property bool arrowToLeft: false
	property alias backgroundColor: collapsibleContentBackground.color
	readonly property alias content: contentItem.children
	property int contentBottomMargin: Style.dimens.groupbox_spacing
	property int contentHorizontalMargin: horizontalMargin
	property alias contentSpacing: contentItem.spacing
	property int contentTopMargin: Style.dimens.groupbox_spacing
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	default property alias expandableData: contentItem.data
	readonly property alias expanded: expandButton.checked
	property int horizontalMargin: Style.dimens.pane_spacing
	property alias selectionIcon: selectionIcon.source
	property alias selectionTitle: selectionTitle.text
	property bool startExpanded: false
	property alias tintIcon: selectionIcon.tintEnabled
	property alias title: title.text

	function onOptionSelected() {
		if (expandButton.checked && !ApplicationModel.screenReaderRunning) {
			expandButton.checked = false;
		}
	}

	spacing: 0

	GAbstractButton {
		id: expandButton

		//: ALL_PLATFORMS
		Accessible.description: root.selectionTitle !== "" ? qsTr("Currently selected is %1").arg(root.selectionTitle) : ""
		Accessible.name: root.title
		Accessible.role: {
			if ("Switch" in Accessible) {
				return Accessible.Switch; // qmllint disable missing-property
			}
			return Accessible.Button;
		}
		checkable: true
		implicitHeight: bannerLayout.implicitHeight + Style.dimens.pane_spacing * 2
		implicitWidth: bannerLayout.implicitWidth

		background: RoundedRectangle {
			id: background

			bottomLeftCorner: root.drawBottomCorners && !expandButton.checked
			bottomRightCorner: root.drawBottomCorners && !expandButton.checked
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
					id: selectionTitle

					Accessible.ignored: true
					visible: root.alwaysReserveSelectionTitleHeight || text !== ""

					Behavior on text {
						SequentialAnimation {
							PropertyAnimation {
								duration: Style.animation_duration
								easing.type: Easing.InCubic
								property: "opacity"
								target: selectionTitle
								to: 0
							}
							PropertyAction {
								property: "text"
								target: selectionTitle
							}
							PropertyAnimation {
								duration: Style.animation_duration
								easing.type: Easing.OutCubic
								property: "opacity"
								target: selectionTitle
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
				id: selectionIcon

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

		Accessible.onPressAction: expandButton.toggle()
		Accessible.onScrollDownAction: Utils.scrollPageDownOnGFlickable(this)
		Accessible.onScrollUpAction: Utils.scrollPageUpOnGFlickable(this)
		Component.onCompleted: checked = root.startExpanded

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
		implicitHeight: expandButton.checked ? (contentItem.implicitHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin) : 0
		implicitWidth: contentItem.implicitWidth + contentItem.anchors.leftMargin + contentItem.anchors.rightMargin
		topLeftCorner: false
		topRightCorner: false
		visible: expandButton.checked

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
		source: expandButton.checked ? "qrc:///images/material_expand_less.svg" : "qrc:///images/material_expand_more.svg"
		sourceSize.height: Style.text.normal.textSize
		tintColor: Style.color.textNormal.basic_unchecked
		tintEnabled: true
	}
}
