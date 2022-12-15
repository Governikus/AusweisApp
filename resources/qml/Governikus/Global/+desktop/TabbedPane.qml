/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0

Item {
	id: root

	readonly property int availableHeight: height - 2 * contentPadding - 2 * Style.dimens.high_contrast_item_border
	property Component contentDelegate: null
	property var contentObjectModel: undefined
	property int contentPadding: Constants.pane_padding
	readonly property var currentContentItem: contentLoader.item
	property alias currentIndex: sectionNameList.currentIndex
	readonly property var currentItemModel: sectionNameList.currentItem ? sectionNameList.currentItem.itemModel : null
	property Component footerItem: null
	readonly property real relativeListViewWidth: 0.3
	property alias sectionCount: sectionNameList.count
	property Component sectionDelegate: TabbedPaneDelegateText {
		sectionName: model ? (model.display ? model.display : model.modelData) : ""
	}
	property var sectionsModel: undefined

	function scrollYPositionIntoView(pYposition) {
		var dy = pYposition - flickable.contentY - flickable.height;
		if (dy > 0 || flickable.contentY > 0) {
			flickable.contentY += dy;
			if (flickable.contentY < 0)
				flickable.contentY = 0;
			else if (flickable.contentY + flickable.height > flickable.contentHeight)
				flickable.contentY = flickable.contentHeight - flickable.height;
		}
	}

	Item {
		anchors.fill: parent

		ColumnLayout {
			height: parent.height
			spacing: Constants.component_spacing
			width: parent.width * relativeListViewWidth + 2 * Style.dimens.high_contrast_item_border
			z: 1

			anchors {
				bottom: parent.bottom
				left: parent.left
				top: parent.top
			}
			GListView {
				id: sectionNameList
				Layout.fillHeight: true
				Layout.fillWidth: true
				activeFocusOnTab: true
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				delegate: sectionNameDelegate
				highlight: null
				highlightFollowsCurrentItem: true
				model: sectionsModel
				scrollBarAutohide: true
				scrollBarBottomPadding: Constants.text_spacing
				scrollBarTopPadding: Constants.text_spacing

				onCurrentIndexChanged: flickable.positionViewAtBeginning()
			}
			Loader {
				Layout.alignment: Qt.AlignLeft
				Layout.fillWidth: true
				Layout.preferredHeight: status === Loader.Ready ? item.height : 0
				sourceComponent: footerItem
			}
		}
		RoundedRectangle {
			anchors.right: parent.right
			borderColor: Style.color.high_contrast_item_border
			borderWidth: Style.dimens.high_contrast_item_border
			bottomLeftCorner: sectionNameList.contentHeight < height
			color: Style.color.background_pane
			height: parent.height
			radius: Style.dimens.corner_radius
			topLeftCorner: false
			width: parent.width * (1.0 - relativeListViewWidth)

			Item {
				anchors.fill: parent
				anchors.margins: Style.dimens.high_contrast_item_border
				clip: true

				GFlickable {
					id: flickable
					contentHeight: contentLoader.height

					anchors {
						bottomMargin: contentPadding
						fill: parent
						topMargin: contentPadding
					}
					Loader {
						id: contentLoader
						sourceComponent: {
							if (contentDelegate !== null) {
								return contentDelegate;
							}
							if (contentObjectModel === undefined) {
								return null;
							}
							if (sectionNameList.currentIndex < contentObjectModel.count) {
								return contentObjectModel.get(sectionNameList.currentIndex);
							}
						}

						anchors {
							left: parent.left
							leftMargin: contentPadding
							right: parent.right
							rightMargin: contentPadding
							top: parent.top
						}
					}
				}
			}
			ScrollGradients {
				anchors.fill: parent
				anchors.margins: Style.dimens.high_contrast_item_border
				color: parent.color
			}
		}
	}
	Component {
		id: sectionNameDelegate
		Item {
			id: delegateItem

			readonly property bool isCurrentItem: ListView.isCurrentItem
			readonly property bool isFirstItem: index === 0
			readonly property bool isLastItem: index === ListView.view.count - 1
			readonly property bool isPreviousToCurrentItem: index === ListView.view.currentIndex - 1
			readonly property var itemModel: model

			Accessible.focusable: true
			Accessible.name: delegateLoader.item ? delegateLoader.item.sectionName : ""
			Accessible.role: Accessible.Button
			activeFocusOnTab: false
			height: delegateLoader.height + 2 * Constants.pane_padding
			width: sectionNameList.width

			RoundedRectangle {
				id: background
				borderColor: Style.color.high_contrast_item_border
				borderWidth: Style.dimens.high_contrast_item_border
				bottomLeftCorner: isLastItem
				bottomRightCorner: false
				color: isCurrentItem ? Style.color.background_pane : Style.color.background_pane_inactive
				radius: Style.dimens.corner_radius
				topLeftCorner: isFirstItem
				topRightCorner: false

				anchors {
					bottomMargin: isCurrentItem && !isLastItem ? Style.dimens.tabbed_pane_separator_size : 0
					fill: parent
					rightMargin: !isCurrentItem ? (Style.dimens.tabbed_pane_separator_size + 2 * Style.dimens.high_contrast_item_border) : 0
					topMargin: isCurrentItem && !isFirstItem ? Style.dimens.tabbed_pane_separator_size : 0
				}
				Rectangle {
					color: background.color
					width: isCurrentItem ? Style.dimens.high_contrast_item_border : 0

					anchors {
						bottom: parent.bottom
						bottomMargin: isLastItem ? Style.dimens.high_contrast_item_border : 0
						right: parent.right
						top: parent.top
						topMargin: isFirstItem ? Style.dimens.high_contrast_item_border : 0
					}
				}
			}
			GSeparator {
				id: horizontalSeparator
				visible: !isLastItem && !isCurrentItem && !isPreviousToCurrentItem

				anchors {
					bottom: parent.bottom
					left: parent.left
					leftMargin: Style.dimens.high_contrast_item_border
					right: parent.right
					rightMargin: (!isCurrentItem ? Style.dimens.tabbed_pane_separator_size : 0) + 2 * Style.dimens.high_contrast_item_border
				}
			}
			FocusFrame {
				framee: delegateLoader
			}
			Loader {
				id: delegateLoader

				property bool isCurrentItem: parent.isCurrentItem
				property var model: itemModel

				sourceComponent: sectionDelegate

				anchors {
					left: parent.left
					leftMargin: Constants.pane_padding
					right: parent.right
					rightMargin: Constants.pane_padding + 2 * Style.dimens.high_contrast_item_border
					verticalCenter: parent.verticalCenter
				}
			}
			MouseArea {
				anchors.fill: parent
				cursorShape: index === currentIndex ? Qt.ArrowCursor : Qt.PointingHandCursor

				onClicked: {
					delegateItem.ListView.view.currentIndex = index;
				}
			}
		}
	}
}
