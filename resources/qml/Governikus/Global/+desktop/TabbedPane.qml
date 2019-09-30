/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0


Item {
	id: root

	property var sectionsModel: undefined
	property var contentObjectModel: undefined
	property Component contentDelegate: null
	property Component footerItem: null
	property Component sectionDelegate: TabbedPaneDelegateOneLineText {
		sectionName: model ? (model.display ? model.display : model.modelData) : ""
	}
	property alias currentIndex: sectionNameList.currentIndex
	property alias sectionCount: sectionNameList.count

	readonly property var currentItemModel: sectionNameList.currentItem ? sectionNameList.currentItem.itemModel : null
	readonly property int horizonalSeparatorHeight: Math.max(ApplicationModel.scaleFactor * 1, 1)
	readonly property int verticalSeparatorWidth: Math.max(ApplicationModel.scaleFactor * 4, 1)
	readonly property real relativeListViewWidth: 0.3
	readonly property int availableHeight: height - 2 * contentPadding
	property var contentPadding: Constants.pane_padding

	function scrollYPositionIntoView(pYposition) {
		var dy = pYposition - flickable.contentY - flickable.height

		if (dy > 0 || flickable.contentY > 0) {
			flickable.contentY += dy

			if (flickable.contentY < 0)
				flickable.contentY = 0
			else if (flickable.contentY + flickable.height > flickable.contentHeight)
				pFlickable.contentY = flickable.contentHeight - flickable.height
		}
	}

	Row {
		anchors.fill: parent

		ColumnLayout {
			height: parent.height
			width: parent.width * relativeListViewWidth

			spacing: Constants.component_spacing

			ListView {
				id: sectionNameList

				Layout.fillWidth: true
				Layout.fillHeight: true

				activeFocusOnTab: true

				clip: true
				boundsBehavior: Flickable.StopAtBounds
				model: sectionsModel
				highlightFollowsCurrentItem: true
				highlight: null
				delegate: sectionNameDelegate
			}

			Loader {
				Layout.preferredHeight: status === Loader.Ready ? item.height : 0
				Layout.fillWidth: true
				Layout.alignment: Qt.AlignLeft

				sourceComponent: footerItem
			}
		}

		RoundedRectangle {
			height: parent.height
			width: parent.width * (1.0 - relativeListViewWidth)

			radius: Style.dimens.corner_radius
			color: Style.color.background_pane_active
			topLeftCorner: false
			bottomLeftCorner: sectionNameList.contentHeight < height
			clip: true

			Flickable {
				id: flickable

				anchors {
					fill: parent
					bottomMargin: contentPadding
					topMargin: contentPadding
				}

				ScrollBar.vertical: ScrollBar {
					policy: size === 1.0 ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
				}
				contentHeight: contentLoader.item.height
				boundsBehavior: Flickable.StopAtBounds

				Loader {
					id: contentLoader

					anchors {
						top: parent.top
						left: parent.left
						right: parent.right
						leftMargin: contentPadding
						rightMargin: contentPadding
					}

					sourceComponent: {
						if (contentDelegate !== null) {
							return contentDelegate
						}

						if (contentObjectModel === undefined) {
							return null
						}

						if (sectionNameList.currentIndex < contentObjectModel.count){
							return contentObjectModel.get(sectionNameList.currentIndex)
						}
					}
				}
			}

			ScrollGradients {
				anchors.fill: parent
				color: parent.color
			}
		}
	}


	Component {
		id: sectionNameDelegate

		Control {
			id: delegateItem

			readonly property bool isFirstItem: index === 0
			readonly property bool isLastItem: index === ListView.view.count - 1
			readonly property bool isCurrentItem: ListView.isCurrentItem
			readonly property bool isPreviousToCurrentItem: index === ListView.view.currentIndex - 1
			readonly property var itemModel: model

			Accessible.role: Accessible.Grouping

			width: parent.width
			height: delegateLoader.height + 2 * Constants.pane_padding

			RoundedRectangle {
				id: background

				anchors.fill: parent

				radius: Style.dimens.corner_radius
				color: isCurrentItem ? Style.color.background_pane_active : Style.color.background_pane

				topLeftCorner: isFirstItem
				topRightCorner: false
				bottomRightCorner: false
				bottomLeftCorner: isLastItem
			}

			Rectangle {
				id: horizontalSeparator

				visible: !isLastItem

				width: parent.width
				height: isCurrentItem || isPreviousToCurrentItem ? verticalSeparatorWidth : horizonalSeparatorHeight
				anchors.bottom: parent.bottom

				color: isCurrentItem || isPreviousToCurrentItem ? Style.color.background : Style.color.border
			}

			Rectangle {
				id: verticalSeparator

				height: parent.height
				width: verticalSeparatorWidth
				anchors.right: parent.right

				color: !isCurrentItem ? Style.color.background : Style.color.background_pane_active
			}

			Rectangle {
				id: missingPiece

				visible: !isLastItem

				height: isCurrentItem ? verticalSeparatorWidth : horizonalSeparatorHeight
				width: verticalSeparatorWidth
				anchors {
					right: parent.right
					bottom: parent.bottom
				}

				color: Style.color.background
			}

			FocusFrame {
				visible: delegateItem.focusReason !== Qt.MouseFocusReason

				framee: delegateLoader
				border.color: Constants.black
				dynamic: false
			}

			Loader {
				id: delegateLoader

				property var model: itemModel

				anchors {
					verticalCenter: parent.verticalCenter
					left: parent.left
					right: parent.right
					leftMargin: Constants.pane_padding
					rightMargin: Constants.pane_padding
				}

				sourceComponent: sectionDelegate
			}

			MouseArea {
				anchors.fill: parent

				cursorShape: index === currentIndex ? Qt.ArrowCursor : Qt.PointingHandCursor
				onClicked: {
					delegateItem.ListView.view.currentIndex = index
				}
			}
		}
	}
}
