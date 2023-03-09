/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
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

	property real additionalProviderListTopPadding: 0
	readonly property bool additionalResultsItemVisible: ProviderCategoryFilterModel.additionalResultCount > 0 && ProviderCategoryFilterModel.categories.length > 0 && ProviderCategoryFilterModel.categories.indexOf("all") === -1
	readonly property string category: ProviderCategoryFilterModel.categories.length === 0 ? "" : ProviderCategoryFilterModel.categories[0]
	property var headerComponent: null
	property alias headerItem: providerList.headerItem
	property var searchBar
	readonly property bool showCategories: category === ""

	sectionPageFlickable: providerList
	title: Category.displayString(category)
	titleBarColor: Category.displayColor(category)

	navigationAction: NavigationAction {
		action: category !== "" ? NavigationAction.Action.Back : NavigationAction.Action.None

		onClicked: {
			if (category !== "") {
				ProviderCategoryFilterModel.setCategorySelection("");
			} else if (Qt.platform.os !== "osx") {
				reset();
				show(UiModule.DEFAULT);
			}
		}
	}

	onCategoryChanged: highlightScrollbar()
	onReset: {
		ProviderCategoryFilterModel.setCategorySelection("");
		searchBar.reset();
	}
	onShowCategoriesChanged: {
		ProviderCategoryFilterModel.setIncludeCategoriesInModel(showCategories);
		ProviderCategoryFilterModel.sortByCategoryFirst(showCategories);
	}

	Connections {
		function onSearchStringChanged() {
			sectionPageFlickable.positionViewAtBeginning();
		}

		target: ProviderCategoryFilterModel
	}
	Connections {
		function onSearchTextChanged() {
			ProviderCategoryFilterModel.searchString = searchBar.searchText;
		}

		target: searchBar
	}
	Component {
		id: providerDetailView
		ProviderDetailView {
		}
	}
	GText {
		anchors.centerIn: parent
		//: LABEL IOS_PHONE ANDROID_PHONE The text entered into the provider search field results in no matches
		text: qsTr("No results matching your search query found")
		visible: ProviderCategoryFilterModel.rowCount === 0 && ProviderCategoryFilterModel.additionalResultCount === 0
	}
	GListView {
		id: providerList
		Accessible.role: Accessible.List
		anchors.bottom: parent.bottom
		header: headerComponent
		height: contentBehindTitlebar ? (parent.height + Style.dimens.titlebar_height) : (parent.height - additionalProviderListTopPadding)
		model: ProviderCategoryFilterModel
		scrollBarTopPadding: contentBehindTitlebar ? Style.dimens.titlebar_height : 0
		width: parent.width

		delegate: ProviderListItemDelegate {
			showSeparator: index < providerList.count - 1 || additionalResultsItemVisible

			Accessible.onScrollDownAction: providerList.scrollPageDown()
			Accessible.onScrollUpAction: providerList.scrollPageUp()
			onClicked: isProvider ? push(providerDetailView, {
					"providerModelItem": model
				}) : ProviderCategoryFilterModel.setCategorySelection(providerCategory)
		}
		footer: Component {
			Item {
				height: additionalResults.height + separator.height
				width: parent.width

				AdditionalResultsItem {
					id: additionalResults
					height: visible ? Style.dimens.list_item_height : 0
					totalHits: ProviderCategoryFilterModel.additionalResultCount
					visible: additionalResultsItemVisible

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
