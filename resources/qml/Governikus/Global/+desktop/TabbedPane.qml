/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel
import Governikus.View

Item {
	id: root

	readonly property int availableHeight: Math.floor(height - 2 * Constants.pane_spacing)
	property Component contentDelegate: null
	property var contentObjectModel: undefined
	property alias contentRightMargin: flickable.rightMargin
	readonly property var currentContentItem: contentLoader.item
	property alias currentIndex: sectionNameList.currentIndex
	readonly property var currentItemModel: sectionNameList.currentItem ? sectionNameList.currentItem.itemModel : null
	property Component footerItem: null
	property int highlightedIndex: -1
	readonly property real relativeListViewWidth: 0.3
	property alias sectionCount: sectionNameList.count
	property var sectionsModel: undefined

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
		anchors.bottomMargin: Constants.pane_padding
		anchors.fill: parent
		anchors.leftMargin: Constants.pane_padding
		anchors.topMargin: Constants.pane_padding
		opacity: SettingsModel.showBetaTesting ? 0.9 : 1.0

		ColumnLayout {
			id: leftSide

			height: parent.height
			spacing: Constants.component_spacing
			width: parent.width * relativeListViewWidth
			z: 1

			anchors {
				bottom: parent.bottom
				left: parent.left
				top: parent.top
			}
			Item {
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
					delegate: sectionNameDelegate
					highlight: null
					highlightFollowsCurrentItem: true
					model: sectionsModel
					scrollBarAutohide: true
					scrollBarBottomPadding: Constants.text_spacing
					scrollBarTopPadding: Constants.text_spacing

					onCurrentIndexChanged: flickable.positionViewAtBeginning()
				}
			}
			Loader {
				Layout.alignment: Qt.AlignLeft
				Layout.fillWidth: true
				Layout.preferredHeight: status === Loader.Ready ? item.height : 0
				sourceComponent: footerItem
			}
		}
		GFlickableColumnLayout {
			id: flickable

			bottomMargin: 0
			leftMargin: 0
			topMargin: 0

			anchors {
				bottom: parent.bottom
				left: leftSide.right
				leftMargin: Constants.pane_padding
				right: parent.right
				top: parent.top
			}
			Loader {
				id: contentLoader

				Layout.fillWidth: true
				Layout.maximumHeight: item ? item.Layout.maximumHeight : -1
				Layout.maximumWidth: item ? item.Layout.maximumWidth : -1
				Layout.minimumHeight: item ? item.Layout.minimumHeight : -1
				Layout.minimumWidth: item ? item.Layout.minimumWidth : -1
				Layout.preferredHeight: item ? item.Layout.preferredHeight : -1
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
			}
		}
	}
	Component {
		id: sectionNameDelegate

		Item {
			id: delegateItem

			//: LABEL DESKTOP Additional description of TabbedPane behavior for a11y.
			readonly property string a11yDescription: qsTr("You may navigate to different tabs by using the up/down arrows.")
			readonly property bool isFirstItem: index === 0
			readonly property bool isLastItem: index === ListView.view.count - 1
			readonly property var itemModel: model
			readonly property bool nextItemIsHighlighted: index === ListView.view.currentIndex - 1 || index === root.highlightedIndex - 1

			Accessible.description: a11yDescription
			Accessible.focusable: true
			Accessible.name: {
				if (Qt.platform.os === "windows") {
					return sectionName.text + ", " + a11yDescription;
				}
				return sectionName.text + ", " + qsTr("Tab selected");
			}
			Accessible.role: Accessible.PageTab
			activeFocusOnTab: false
			height: sectionName.height + 2 * Constants.pane_padding
			width: sectionNameList.width

			states: [
				State {
					name: "pressed"
					when: mouseArea.pressed

					PropertyChanges {
						background.borderColor: Style.color.pane_border_pressed
						background.color: Style.color.pane_pressed
						horizontalSeparator.visible: false
						sectionName.color: Style.color.text_pressed
					}
				},
				State {
					name: "hovered"
					when: mouseArea.containsMouse

					PropertyChanges {
						background.borderColor: Style.color.pane_border_hovered
						background.color: Style.color.pane_hovered
						horizontalSeparator.visible: false
						sectionName.color: Style.color.text_hovered
					}
				},
				State {
					name: "active"
					when: delegateItem.ListView.isCurrentItem

					PropertyChanges {
						background.borderColor: Style.color.pane_border_active
						background.color: Style.color.pane_active
						horizontalSeparator.visible: false
						sectionName.color: Style.color.text_active
					}
				}
			]

			RoundedRectangle {
				id: background

				anchors.fill: parent
				borderColor: Style.color.pane_border
				bottomLeftCorner: isLastItem
				bottomRightCorner: isLastItem
				color: Style.color.pane
				radius: Style.dimens.pane_radius
				topLeftCorner: isFirstItem
				topRightCorner: isFirstItem
			}
			GText {
				id: sectionName

				color: Style.color.text
				elide: Text.ElideRight
				maximumLineCount: 2
				text: model.display ? model.display : model.modelData
				textStyle: Style.text.subline

				anchors {
					left: parent.left
					leftMargin: Constants.pane_padding
					right: parent.right
					rightMargin: Constants.pane_padding
					verticalCenter: parent.verticalCenter
				}
			}
			GSeparator {
				id: horizontalSeparator

				visible: !isLastItem && !nextItemIsHighlighted && GraphicsInfo.api !== GraphicsInfo.Software

				anchors {
					bottom: parent.bottom
					left: parent.left
					leftMargin: Constants.pane_padding
					right: parent.right
					rightMargin: Constants.pane_padding
				}
			}
			FocusFrame {
				borderColor: sectionName.color
				framee: sectionName
			}
			MouseArea {
				id: mouseArea

				function updateHighlight() {
					if (containsMouse || pressed) {
						root.highlightedIndex = index;
					} else {
						if (root.highlightedIndex === index) {
							root.highlightedIndex = -1;
						}
					}
				}

				anchors.fill: parent
				hoverEnabled: true

				onClicked: {
					delegateItem.ListView.view.itemAtIndex(index).forceActiveFocus(Qt.MouseFocusReason);
					delegateItem.ListView.view.currentIndex = index;
				}
				onContainsMouseChanged: updateHighlight()
				onPressedChanged: updateHighlight()
			}
		}
	}
}
