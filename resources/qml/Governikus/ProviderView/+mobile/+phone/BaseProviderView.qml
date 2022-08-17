/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: baseItem

	onReset: {
		ProviderCategoryFilterModel.setCategorySelection("")
		searchBar.reset()
	}

	readonly property string category: ProviderCategoryFilterModel.categories.length === 0 ? "" : ProviderCategoryFilterModel.categories[0]
	readonly property bool showCategories: category === ""
	readonly property bool additionalResultsItemVisible: ProviderCategoryFilterModel.additionalResultCount > 0 && ProviderCategoryFilterModel.categories.length > 0 && ProviderCategoryFilterModel.categories.indexOf("all") === -1
	property alias headerItem: providerList.headerItem
	property var headerComponent: null
	property var searchBar
	property real additionalProviderListTopPadding: 0

	navigationAction: NavigationAction {
		action: category !== "" ? NavigationAction.Action.Back : NavigationAction.Action.None
		onClicked: {
			if (category !== "") {
				ProviderCategoryFilterModel.setCategorySelection("")
			} else if (Qt.platform.os !== "osx") {
				reset()
				show(UiModule.DEFAULT)
			}
		}
	}

	title: Category.displayString(category)
	titleBarColor: Category.displayColor(category)
	sectionPageFlickable: providerList

	Connections {
		target: ProviderCategoryFilterModel
		function onSearchStringChanged() { sectionPageFlickable.positionViewAtBeginning() }
	}

	Connections {
		target: searchBar
		function onSearchTextChanged() { ProviderCategoryFilterModel.searchString = searchBar.searchText }
	}

	onShowCategoriesChanged: {
		ProviderCategoryFilterModel.setIncludeCategoriesInModel(showCategories)
		ProviderCategoryFilterModel.sortByCategoryFirst(showCategories)
	}

	onCategoryChanged: highlightScrollbar()

	Component {
		id: providerDetailView

		ProviderDetailView {}
	}

	GText {
		visible: ProviderCategoryFilterModel.rowCount === 0 && ProviderCategoryFilterModel.additionalResultCount === 0
		anchors.centerIn: parent
		//: LABEL IOS_PHONE ANDROID_PHONE The text entered into the provider search field results in no matches
		text: qsTr("No results matching your search query found")
	}

	GListView {
		id: providerList

		height: contentBehindTitlebar ? (parent.height + Style.dimens.titlebar_height) : (parent.height - additionalProviderListTopPadding)
		width: parent.width
		anchors.bottom: parent.bottom

		Accessible.role: Accessible.List

		model: ProviderCategoryFilterModel

		scrollBarTopPadding: contentBehindTitlebar ? Style.dimens.titlebar_height : 0

		delegate: ProviderListItemDelegate {
			Accessible.onScrollDownAction: providerList.scrollPageDown()
			Accessible.onScrollUpAction: providerList.scrollPageUp()

			showSeparator: index < providerList.count - 1  || additionalResultsItemVisible

			onClicked: isProvider ? push(providerDetailView, {providerModelItem: model}) : ProviderCategoryFilterModel.setCategorySelection(providerCategory)
		}

		header: headerComponent

		footer: Component {
			Item {
				width: parent.width
				height: additionalResults.height + separator.height

				AdditionalResultsItem {
					id: additionalResults

					visible: additionalResultsItemVisible
					height: visible ? Style.dimens.list_item_height : 0

					totalHits: ProviderCategoryFilterModel.additionalResultCount

					onClicked: ProviderCategoryFilterModel.setCategorySelection("")
				}

				GSeparator {
					id: separator

					anchors.top: additionalResults.bottom
					width: parent.width
				}
			}
		}
	}
}
