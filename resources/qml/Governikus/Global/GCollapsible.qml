/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

ColumnLayout {
	id: root

	property bool alwaysReserveSelectionTitleHight: false
	property alias backgroundColor: background.color
	property int contentBottomMargin: Constants.groupbox_spacing
	property alias contentSpacing: contentItem.spacing
	property int contentTopMargin: Constants.groupbox_spacing
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	default property alias expandableData: contentItem.data
	property bool expanded: false
	property int horizontalMargin: Constants.component_spacing
	property alias selectionIcon: selectionIcon.source
	property alias selectionTitle: selectionTitle.text
	property alias tintIcon: selectionIcon.tintEnabled
	property alias title: title.text

	spacing: 0

	AbstractButton {
		id: expandButton

		Accessible.name: root.title + ". " +
		//: LABEL ANDROID IOS
		(expanded ? qsTr("collapse") :
			//: LABEL ANDROID IOS
			qsTr("expand") + ". ") +
		//: LABEL ANDROID IOS
		(root.selectionTitle !== "" ? qsTr("Currently selected is %1").arg(root.selectionTitle) : "")
		Accessible.role: Accessible.Button
		Layout.fillWidth: true
		implicitHeight: bannerLayout.implicitHeight + Constants.component_spacing * 2
		implicitWidth: bannerLayout.implicitWidth

		background: RoundedRectangle {
			bottomLeftCorner: drawBottomCorners && !expanded
			bottomRightCorner: drawBottomCorners && !expanded
			color: expandButton.pressed ? Style.color.pane_active : Style.color.transparent
			topLeftCorner: drawTopCorners
			topRightCorner: drawTopCorners
		}
		contentItem: RowLayout {
			id: bannerLayout

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			spacing: 0

			ColumnLayout {
				Layout.leftMargin: horizontalMargin
				spacing: Constants.subtext_spacing

				GText {
					id: title

					Accessible.ignored: true
					color: expandButton.pressed ? Style.color.text_subline_pressed : textStyle.textColor
					textStyle: Style.text.subline
					visible: text !== ""
				}
				GText {
					id: selectionTitle

					Accessible.ignored: true
					color: expandButton.pressed ? Style.color.text_pressed : textStyle.textColor
					visible: alwaysReserveSelectionTitleHight || text !== ""

					Behavior on text {
						SequentialAnimation {
							PropertyAnimation {
								duration: Constants.animation_duration
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
								duration: Constants.animation_duration
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

				Layout.maximumHeight: Style.dimens.small_icon_size
				Layout.maximumWidth: Math.ceil(paintedWidth)
				Layout.preferredHeight: Style.dimens.small_icon_size
				tintColor: arrow.tintColor
				tintEnabled: false
			}
			TintableIcon {
				id: arrow

				Layout.maximumHeight: Style.dimens.small_icon_size
				Layout.maximumWidth: Layout.maximumHeight
				Layout.preferredHeight: Style.dimens.small_icon_size
				Layout.preferredWidth: Layout.preferredHeight
				Layout.rightMargin: horizontalMargin
				source: expanded ? "qrc:///images/material_expand_less.svg" : "qrc:///images/material_expand_more.svg"
				tintColor: expandButton.pressed ? Style.color.text_pressed : Style.color.text
				tintEnabled: true
			}
		}

		onClicked: expanded = !expanded
	}
	RoundedRectangle {
		id: background

		Layout.fillWidth: true
		bottomLeftCorner: drawBottomCorners
		bottomRightCorner: drawBottomCorners
		clip: true
		color: Style.color.pane_sublevel
		implicitHeight: expanded ? (contentItem.implicitHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin) : 0
		implicitWidth: contentItem.implicitWidth + contentItem.anchors.leftMargin + contentItem.anchors.rightMargin
		topLeftCorner: false
		topRightCorner: false

		Behavior on implicitHeight {
			NumberAnimation {
				duration: Constants.animation_duration
			}
		}

		ColumnLayout {
			id: contentItem

			spacing: Constants.groupbox_spacing

			anchors {
				bottomMargin: contentBottomMargin
				fill: parent
				leftMargin: root.horizontalMargin
				rightMargin: root.horizontalMargin
				topMargin: contentTopMargin
			}
		}
	}
}
