/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

Item {
	id: root

	readonly property int availableHeight: Math.floor(height - 2 * Style.dimens.pane_spacing)
	property Component contentDelegate: null
	property var contentObjectModel: undefined
	property alias contentRightMargin: flickable.rightMargin
	readonly property var currentContentItem: contentLoader.item
	property alias currentIndex: sectionNameList.currentIndex
	readonly property var currentItemModel: (sectionNameList.currentItem as TabbedPaneDelegate) ? (sectionNameList.currentItem as TabbedPaneDelegate).model : null
	property Component footerItem: null
	readonly property real relativeListViewWidth: 0.3
	property alias sectionCount: sectionNameList.count
	property var sectionsModel: undefined

	function handleKeyPress(key) {
		if (currentContentItem instanceof GListView) {
			currentContentItem.handleKeyPress(key);
			return;
		}
		flickable.handleKeyPress(key);
	}
	function scrollYPositionIntoView(pYposition) {
		let dy = pYposition - flickable.contentY - flickable.height;
		if (dy > 0 || flickable.contentY > 0) {
			flickable.contentY += dy;
			if (flickable.contentY < 0)
				flickable.contentY = 0;
			else if (flickable.contentY + flickable.height > flickable.contentHeight)
				flickable.contentY = flickable.contentHeight - flickable.height;
		}
	}

	Item {
		anchors.bottomMargin: Style.dimens.pane_padding
		anchors.fill: parent
		anchors.leftMargin: Style.dimens.pane_padding
		anchors.topMargin: Style.dimens.pane_padding
		opacity: SettingsModel.showBetaTesting ? 0.9 : 1.0

		ColumnLayout {
			id: leftSide

			height: parent.height
			spacing: Style.dimens.pane_spacing
			width: parent.width * root.relativeListViewWidth
			z: 1

			anchors {
				bottom: parent.bottom
				left: parent.left
				top: parent.top
			}
			Item {
				//: LABEL DESKTOP
				Accessible.name: qsTr("Sidebar")
				Accessible.role: Accessible.PageTabList
				Layout.fillHeight: true
				Layout.fillWidth: true

				layer {
					enabled: GraphicsInfo.api !== GraphicsInfo.Software

					effect: GDropShadow {
					}
				}
				GListView {
					id: sectionNameList

					activeFocusOnTab: true
					anchors.fill: parent
					boundsBehavior: Flickable.StopAtBounds
					clip: true
					highlight: null
					highlightFollowsCurrentItem: true
					model: root.sectionsModel
					scrollBarAutohide: true
					scrollBarBottomPadding: Style.dimens.text_spacing
					scrollBarTopPadding: Style.dimens.text_spacing

					delegate: TabbedPaneDelegate {
					}

					onCurrentIndexChanged: flickable.positionViewAtBeginning()
				}
			}
			Loader {
				Layout.alignment: Qt.AlignLeft
				Layout.fillWidth: true
				Layout.preferredHeight: status === Loader.Ready ? (item as Item).implicitHeight : 0
				sourceComponent: root.footerItem
			}
		}
		GFlickableColumnLayout {
			id: flickable

			Accessible.ignored: false
			//: LABEL DESKTOP %1 will be replaced with the title of the tab
			Accessible.name: root.sectionsModel ? qsTr("Content of tab \"%1\"").arg(root.sectionsModel[sectionNameList.currentIndex]) : ""
			Accessible.role: Accessible.Grouping
			bottomMargin: 0
			leftMargin: 0
			topMargin: 0

			anchors {
				bottom: parent.bottom
				left: leftSide.right
				leftMargin: Style.dimens.pane_padding
				right: parent.right
				top: parent.top
			}
			Loader {
				id: contentLoader

				Layout.fillWidth: true
				Layout.maximumHeight: item ? (item as Item).Layout.maximumHeight : -1
				Layout.maximumWidth: item ? (item as Item).Layout.maximumWidth : -1
				Layout.minimumHeight: item ? (item as Item).Layout.minimumHeight : -1
				Layout.minimumWidth: item ? (item as Item).Layout.minimumWidth : -1
				Layout.preferredHeight: item ? (item as Item).Layout.preferredHeight : -1
				sourceComponent: {
					if (root.contentDelegate !== null) {
						return root.contentDelegate;
					}
					if (root.contentObjectModel === undefined) {
						return null;
					}
					if (sectionNameList.currentIndex < root.contentObjectModel.count) {
						return root.contentObjectModel.get(sectionNameList.currentIndex);
					}
				}
			}
		}
	}
}
