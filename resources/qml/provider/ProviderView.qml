import QtQuick 2.5

import "../"
import "../global"

SectionPage {
	id: baseItem

	ProviderStyle {
		id: providerStyle
	}

	headerTitleBarAction: TitleBarAction { text: qsTr("Provider"); font.bold: true }

	rightTitleBarAction: TitleBarSearchAction {
		availableWidth: baseItem.width - leftTitleBarAction.contentWidth - Constants.menubar_width
		onSearchTextChanged: baseItem.searchText = searchText
	}

	ProviderSectionView {
		id: categoryProviderView
		visible: false
	}

	ProviderDetailView {
		id: providerDetailView
		visible: false
	}

	property string searchText: ""

	header: ProviderViewHeader {
		id: providerViewHeader

		width: baseItem.width

		onSearchTextChanged: baseItem.searchText = searchText
	}

	content: Column {
		height: Math.max(providerList.height, baseItem.height)
		width: baseItem.width

		Rectangle {
			id: allSection

			width: baseItem.width
			height: Constants.provider_section_height
			color: "white"
			visible: baseItem.searchText === ""

			Item {
				anchors.fill: parent
				anchors.topMargin: Utils.dp(5)
				anchors.bottomMargin: Utils.dp(5)

				Rectangle {
					id: iconBackground

					anchors.centerIn: allImage
					height: allImage.height
					width: height
					radius: width * 0.5

					border.width: 0
					color: Constants.all_image_background_color
				}


				Image {
					id: allImage
					source: "qrc:///images/All.png"
					anchors.left: parent.left
					anchors.verticalCenter: parent.verticalCenter
					anchors.leftMargin: providerStyle.leftIconMargin
					height: parent.height
					width: parent.width * 0.15
					fillMode: Image.PreserveAspectFit
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: allImage.right
					anchors.leftMargin: providerStyle.leftProviderListMargin
					font.pixelSize: providerStyle.categoryFontPixelSize
					font.bold: providerStyle.categoryFontBold
					color: providerStyle.categoryColor
					elide: Text.ElideRight
					text: qsTr("All")
				}

				Text {
					anchors.right: parent.right
					anchors.rightMargin: Utils.dp(5)
					anchors.verticalCenter: parent.verticalCenter

					text: ">"
					color: Constants.grey
					font.pixelSize: Utils.sp(14)
					visible: providerStyle.showCategoryRightArrow
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						push(categoryProviderView, {selectedCategory: "all"})
					}
				}
			}

			Rectangle {
				width: parent.width * 0.85
				anchors.top: parent.bottom
				anchors.topMargin: -height
				anchors.right: parent.right
				height: 1
				color: Constants.grey
			}
		}

		ListView {
			id: providerList
			height: contentHeight
			width: baseItem.width

			section.property: "category"
			section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart

			model: ProviderDelegateModel {
				model: providerModel.sortModel
				searchText: baseItem.searchText
				delegate: ProviderViewDelegate {
					height: visible ? Constants.provider_section_height : 0
					visible: baseItem.searchText !== ""
				}
			}

			section.delegate: Rectangle {
				id: sectionDelegate
				width: parent.width
				height: Constants.provider_section_height
				visible: baseItem.searchText === ""
				color: "white"
				clip: true

				Item {
					anchors.fill: parent
					anchors.topMargin: Utils.dp(5)
					anchors.bottomMargin: Utils.dp(5)

					Image {
						id: sectionImage
						source: Category.imageSource(section)
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter
						anchors.leftMargin: providerStyle.leftIconMargin
						height: parent.height
						width: parent.width * 0.15
						fillMode: Image.PreserveAspectFit
					}

					Text {
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: sectionImage.right
						anchors.leftMargin: providerStyle.leftProviderListMargin
						color: providerStyle.categoryColor
						font.pixelSize: providerStyle.categoryFontPixelSize
						font.bold: providerStyle.categoryFontBold
						elide: Text.ElideRight
						text: Category.displayString(section)
					}

					Text {
						anchors.right: parent.right
						anchors.rightMargin: Utils.dp(5)
						anchors.verticalCenter: parent.verticalCenter

						text: ">"
						color: Constants.grey
						font.pixelSize: Utils.sp(14)
						visible: providerStyle.showCategoryRightArrow
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							push(categoryProviderView, {selectedCategory: section})
						}
					}
				}

				Rectangle {
					width: parent.width * 0.85
					anchors.top: parent.bottom
					anchors.topMargin: -height
					anchors.right: parent.right
					height: 1
					color: Constants.grey
				}
			}
		}
	}
}
