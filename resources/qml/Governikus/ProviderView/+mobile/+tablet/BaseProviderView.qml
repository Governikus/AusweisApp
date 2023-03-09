/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: baseItem

	property real additionalProviderListTopPadding: 0
	readonly property int headerHeight: 54
	readonly property int separatorHeight: 2
	property bool wasVisible: false

	function pushProviderDetails(pModel) {
		HistoryModel.nameFilter.setProviderAddress(pModel.providerAddress);
		push(providerDetailView, {
				"providerModelItem": pModel
			});
	}

	//: LABEL IOS_TABLET ANDROID_TABLET
	title: qsTr("Provider")
	titleBarColor: Style.color.accent
	visible: false

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: {
			baseItem.reset();
			show(UiModule.DEFAULT);
		}
	}

	onReset: {
		ProviderCategoryFilterModel.setCategorySelection("");
		if (rightTitleBarAction)
			rightTitleBarAction.reset();
	}
	onVisibleChanged: wasVisible = true

	Component {
		id: providerDetailView
		ProviderDetailView {
		}
	}
	Column {
		id: content
		anchors {
			fill: parent
			topMargin: additionalProviderListTopPadding
		}
		Rectangle {
			color: Constants.white
			height: baseItem.headerHeight
			width: parent.width
			z: 1

			Row {
				id: checkBoxesItem
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				height: parent.height
				padding: 30
				scale: Math.min(parent.width / width, 1)
				spacing: 30
				transformOrigin: Item.Center

				CategoryCheckbox {
					id: checkBoxCitizen
					category: "citizen"
					imageSource: Category.imageSource("citizen")
					//: LABEL IOS_TABLET ANDROID_TABLET
					text: qsTr("Citizen services")
				}
				CategoryCheckbox {
					id: checkBoxFinance
					category: "finance"
					imageSource: Category.imageSource("finance")
					//: LABEL IOS_TABLET ANDROID_TABLET
					text: qsTr("Financials")
				}
				CategoryCheckbox {
					id: checkBoxInsurance
					category: "insurance"
					imageSource: Category.imageSource("insurance")
					//: LABEL IOS_TABLET ANDROID_TABLET
					text: qsTr("Insurances")
				}
				CategoryCheckbox {
					id: checkBoxOther
					category: "other"
					imageSource: Category.imageSource("other")
					//: LABEL IOS_TABLET ANDROID_TABLET
					text: qsTr("Other services")
				}
			}
		}
		GSeparator {
			width: parent.width
			z: 1
		}
		ProviderGridView {
			anchors.horizontalCenter: parent.horizontalCenter
			height: baseItem.height - (baseItem.headerHeight + baseItem.separatorHeight) - additionalProviderListTopPadding
			width: parent.width

			onShowAdditionalResults: ProviderCategoryFilterModel.addAdditionalResultCategories()
			onShowDetails: pModelItem => {
				baseItem.pushProviderDetails(pModelItem);
			}
		}
	}
}
