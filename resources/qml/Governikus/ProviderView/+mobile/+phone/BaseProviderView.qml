/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0


SectionPage {
	id: baseItem

	readonly property var category: ProviderCategoryFilterModel.categories.length === 0 ? "" : ProviderCategoryFilterModel.categories[0]

	title: Category.displayString(category) + SettingsModel.translationTrigger
	titleBarColor: Category.displayColor(category)

	navigationAction: NavigationAction {
		state: category !== "" ? "back" : ""
		onClicked: {
			if (state === "back") {
				ProviderCategoryFilterModel.setCategorySelection("")
			}
		}
	}

	Component.onCompleted: ProviderCategoryFilterModel.sortByCategoryFirst(true)

	onCategoryChanged: {
		ProviderCategoryFilterModel.sortByCategoryFirst(category === "")
		highlightScrollbar()
	}

	Component {
		id: providerDetailView

		ProviderDetailView {}
	}

	content: Column {
		width: baseItem.width

		Rectangle {
			visible: ProviderCategoryFilterModel.rowCount === 0 && !additionalResults.visible
			height: 200
			width: parent.width

			color: Style.color.background

			GText {
				anchors.centerIn: parent
				//: LABEL IOS_PHONE ANDROID_PHONE The text entered into the provider search field results in no matches
				text: qsTr("No results matching your search query found") + SettingsModel.translationTrigger
				textStyle: Style.text.normal
			}
		}

		ProviderSectionDelegate {
			id: allSection

			visible: ProviderCategoryFilterModel.searchString === "" && ProviderCategoryFilterModel.categories.length === 0
			height: visible ? Style.dimens.list_item_height : 0

			sectionName: "all"
		}

		GListView {
			id: providerListMain

			visible: category === ""
			height: childrenRect.height
			width: baseItem.width

			Accessible.role: Accessible.List

			scrollBarEnabled: false
			interactive: false
			model: ProviderCategoryFilterModel
			delegate: ProviderListItemDelegate {
				width: visible ? providerListMain.width : 0
				height: visible ? Style.dimens.list_item_height : 0
				visible: ProviderCategoryFilterModel.searchString !== ""
			}

			section.property: "providerCategory"
			section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart
			section.delegate: ProviderSectionDelegate {
				sectionName: section
			}
		}

		GListView {
			id: providerListSection

			visible: !providerListMain.visible
			height: childrenRect.height
			width: baseItem.width

			Accessible.role: Accessible.List

			scrollBarEnabled: false
			interactive: false

			model: ProviderCategoryFilterModel

			delegate: ProviderListItemDelegate {
				width: providerListSection.width

				Accessible.onScrollDownAction: baseItem.scrollPageDown()
				Accessible.onScrollUpAction: baseItem.scrollPageUp()
			}
		}

		AdditionalResultsItem {
			id: additionalResults
			width: parent.width
		}
	}
}
