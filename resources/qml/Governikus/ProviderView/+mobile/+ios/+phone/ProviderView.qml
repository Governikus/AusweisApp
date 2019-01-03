import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

SectionPage {
	id: baseItem

	readonly property var category: ProviderCategoryFilterModel.categories.length === 0 ? "" : ProviderCategoryFilterModel.categories[0]

	Component.onCompleted: ProviderCategoryFilterModel.sortByCategoryFirst(true)

	onCategoryChanged: {
		ProviderCategoryFilterModel.sortByCategoryFirst(category === "")
	}

	leftTitleBarAction: TitleBarAction {
		state: category !== "" ? "back" : ""
		onClicked: {
			if (state === "back") {
				ProviderCategoryFilterModel.setCategorySelection("")
			}
		}
	}

	headerTitleBarAction: TitleBarAction {
		text: Category.displayString(category) + settingsModel.translationTrigger
		font.bold: true
	}

	subTitleBarAction: SearchBar {
		width: baseItem.width
		onSearchTextChanged: ProviderCategoryFilterModel.searchString = searchText
	}

	titleBarColor: Category.displayColor(category)

	ProviderDetailView {
		id: providerDetailView
		visible: false
	}

	content: Column {
		width: baseItem.width

		Rectangle {
			height: Utils.dp(200)
			width: parent.width
			color: Constants.background_color
			visible: ProviderCategoryFilterModel.rowCount === 0 && !additionalResults.visible

			Text {
				color: Constants.secondary_text
				anchors.centerIn: parent
				text: qsTr("No match found") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
			}
		}

		ProviderSectionDelegate {
			id: allSection
			sectionName: "all"
			visible: ProviderCategoryFilterModel.searchString === "" && ProviderCategoryFilterModel.categories.length === 0
			height: visible ? Constants.provider_section_height : 0
		}

		ListView {
			id: providerListMain
			height: childrenRect.height
			width: baseItem.width
			interactive: false
			visible: category === ""

			model: ProviderCategoryFilterModel

			delegate: ProviderViewDelegate {
				height: visible ? Constants.provider_section_height : 0
				visible: ProviderCategoryFilterModel.searchString !== ""
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

			model: ProviderCategoryFilterModel

			delegate: ProviderViewDelegate {}
		}

		AdditionalResultsItem {
			id: additionalResults
			width: parent.width
		}
	}
}
