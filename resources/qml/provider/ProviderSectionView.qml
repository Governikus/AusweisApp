import QtQuick 2.5

import "../"
import "../global"

SectionPage {
	id: baseItem

	ProviderStyle {
		id: providerStyle
	}

	property string searchText: ""
	property string selectedCategory: ""

	onSelectedCategoryChanged: {
		providerModel.filter.setFilterFixedString(selectedCategory === "all" ? "" : selectedCategory)
	}

	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: {baseItem.contentY=0; pop()} }

	headerTitleBarAction: TitleBarAction { text: Category.displayString(selectedCategory); font.bold: true }

	rightTitleBarAction: TitleBarSearchAction {
		availableWidth: baseItem.width - leftTitleBarAction.contentWidth
		onSearchTextChanged: baseItem.searchText = searchText
	}

	titleBarColor: Category.displayColor(selectedCategory)

	header: ProviderViewHeader {
		id: providerViewHeader

		selectedCategory: baseItem.selectedCategory
		withButtons: false
		width: baseItem.width
		onSearchTextChanged: baseItem.searchText = searchText
	}


	content: Item {

		height: Math.max(providerList.height, baseItem.height)
		width: baseItem.width

		Rectangle {
			anchors.fill: providerList
			color: Constants.background_color
		}

		ListView {
			id: providerList

			height: contentHeight
			width: baseItem.width

			section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart

			model: ProviderDelegateModel {
				model: providerModel.filter
				searchText: baseItem.searchText
				delegate: ProviderViewDelegate {
					height: Constants.provider_section_height
				}
			}
		}
	}
}
