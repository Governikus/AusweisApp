/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel

ColumnLayout {
	id: root

	property bool alwaysReserveSelectionTitleHeight: false
	property alias backgroundColor: collapsibleContentBackground.color
	property int contentBottomMargin: Constants.groupbox_spacing
	property int contentHorizontalMargin: horizontalMargin
	property alias contentSpacing: contentItem.spacing
	property int contentTopMargin: Constants.groupbox_spacing
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	default property alias expandableData: contentItem.data
	property bool expanded: false
	readonly property bool expandedOrScreenReaderRunning: expanded || ApplicationModel.isScreenReaderRunning()
	property int horizontalMargin: Constants.component_spacing
	property alias selectionIcon: selectionIcon.source
	property alias selectionTitle: selectionTitle.text
	property alias tintIcon: selectionIcon.tintEnabled
	property alias title: title.text

	spacing: 0

	AbstractButton {
		id: expandButton

		//: LABEL ANDROID IOS
		Accessible.name: "%1 . %2".arg(root.title).arg(root.selectionTitle !== "" ? qsTr("Currently selected is %1").arg(root.selectionTitle) : "")
		Accessible.role: Accessible.StaticText
		Layout.fillWidth: true
		implicitHeight: bannerLayout.implicitHeight + Constants.component_spacing * 2
		implicitWidth: bannerLayout.implicitWidth

		background: RoundedRectangle {
			id: background

			bottomLeftCorner: drawBottomCorners && !expandedOrScreenReaderRunning
			bottomRightCorner: drawBottomCorners && !expandedOrScreenReaderRunning
			color: Style.color.transparent
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
					textStyle: Style.text.subline
					visible: text !== ""
				}
				GText {
					id: selectionTitle

					Accessible.ignored: true
					visible: alwaysReserveSelectionTitleHeight || text !== ""

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
				Layout.preferredHeight: Style.dimens.small_icon_size
				Layout.preferredWidth: Style.dimens.icon_size
				tintColor: Style.color.text
				tintEnabled: false
				visible: source.toString() !== ""
			}
			TintableIcon {
				id: arrow

				Layout.maximumHeight: Style.dimens.small_icon_size
				Layout.maximumWidth: Layout.maximumHeight
				Layout.preferredHeight: Style.dimens.small_icon_size
				Layout.preferredWidth: Layout.preferredHeight
				Layout.rightMargin: horizontalMargin
				source: expandedOrScreenReaderRunning ? "qrc:///images/material_expand_less.svg" : "qrc:///images/material_expand_more.svg"
				tintColor: Style.color.text
				tintEnabled: true
			}
		}

		onClicked: expanded = !expanded
	}
	RoundedRectangle {
		id: collapsibleContentBackground

		Layout.fillWidth: true
		bottomLeftCorner: drawBottomCorners
		bottomRightCorner: drawBottomCorners
		clip: true
		color: Style.color.pane_sublevel
		implicitHeight: expandedOrScreenReaderRunning ? (contentItem.implicitHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin) : 0
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
				leftMargin: contentHorizontalMargin
				rightMargin: contentHorizontalMargin
				topMargin: contentTopMargin
			}
		}
	}
	Item {
		states: [
			State {
				name: "pressed"
				when: expandButton.pressed

				PropertyChanges {
					arrow.tintColor: Style.color.text_pressed
					background.color: Style.color.pane_pressed
					selectionIcon.tintColor: Style.color.text_pressed
					selectionTitle.color: Style.color.text_pressed
					title.color: Style.color.text_subline_pressed
				}
			}
		]
		transitions: [
			EaseInPressedTransition {
				targets: [background, selectionTitle, arrow, selectionIcon, title]
			}
		]
	}
}
