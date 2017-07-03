import QtQuick 2.5

import "../"
import "../../"
import "../../global"

SectionPage {
	id: baseItem

	readonly property var category: providerModel.categories.length === 0 ? "" : providerModel.categories[0]

	Component.onCompleted: providerModel.sortByCategoryFirst(true)

	onCategoryChanged: {
		providerModel.sortByCategoryFirst(category === "")
	}

	ProviderStyle {
		id: providerStyle
	}

	leftTitleBarAction: TitleBarMenuAction {
		state: category !== "" ? "back" : ""
		onClicked: {
			if (state === "back") {
				providerModel.setCategorySelection("")
			}
		}
	}

	headerTitleBarAction: TitleBarAction {
		text: Category.displayString(category)
		font.bold: true
	}

	rightTitleBarAction: SearchBar {
		availableWidth: baseItem.width - Constants.menubar_width
		onSearchTextChanged: providerModel.searchString = searchText
	}

	titleBarColor: Category.displayColor(category)

	ProviderDetailView {
		id: providerDetailView
		visible: false
	}

	header: ProviderHeader {
		width: baseItem.width
		selectedCategory: category
	}

	content: Column {
		width: baseItem.width

		Rectangle {
			height: Utils.dp(200)
			width: parent.width
			color: Constants.background_color
			visible: providerModel.rowCount === 0 && !additionalResults.visible

			Text {
				anchors.centerIn: parent
				text: qsTr("No match found")
				font.pixelSize: Constants.normal_font_size
			}
		}

		ProviderSectionDelegate {
			id: allSection
			sectionName: "all"
			visible: providerModel.searchString === "" && providerModel.categories.length === 0
			height: visible ? Constants.provider_section_height : 0
		}

		ListView {
			id: providerListMain
			height: childrenRect.height
			width: baseItem.width
			interactive: false
			visible: category === ""

			model: providerModel

			delegate: ProviderViewDelegate {
				height: visible ? Constants.provider_section_height : 0
				visible: providerModel.searchString !== ""
			}

			section.property: "providerCategory"
			section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart
			section.delegate: ProviderSectionDelegate {
				sectionName: section
			}
		}

		ListView {
			id: providerListSection
			height: childrenRect.height
			width: baseItem.width
			interactive: false
			visible: !providerListMain.visible

			model: providerModel

			delegate: ProviderViewDelegate {}
		}

		AdditionalResultsItem {
			id: additionalResults
			width: parent.width
		}
	}
}
