/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

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

	onReset: {
		ProviderCategoryFilterModel.setCategorySelection("")
		if (subTitleBarAction) subTitleBarAction.reset()
		if (rightTitleBarAction) rightTitleBarAction.reset()
	}

	//: LABEL IOS_TABLET ANDROID_TABLET
	title: qsTr("Provider")

	titleBarColor: Style.color.accent
	navigationAction: NavigationAction {
		state: "back"
		onClicked: {
			baseItem.reset()
			navBar.show(UiModule.DEFAULT)
		}
	}

	visible: false
	property bool wasVisible: false
	onVisibleChanged: wasVisible = true

	readonly property int headerHeight: 54
	readonly property int separatorHeight: 2
	property real additionalProviderListTopPadding: 0

	function pushProviderDetails(pModel) {
		HistoryModel.nameFilter.setProviderAddress(pModel.providerAddress)
		firePushWithProperties(providerDetailView, {providerModelItem: pModel})
	}

	Component {
		id: providerDetailView

		ProviderDetailView {}
	}

	Column {
		id: content

		anchors {
			fill: parent
			topMargin: additionalProviderListTopPadding
		}

		Rectangle {

			height: baseItem.headerHeight
			width: parent.width
			z: 1

			color: Constants.white

			Row {
				id: checkBoxesItem

				height: parent.height
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter

				padding: 30
				spacing: 30

				transformOrigin: Item.Center
				scale: Math.min(parent.width / width, 1)

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
			height: baseItem.height - (baseItem.headerHeight + baseItem.separatorHeight) - additionalProviderListTopPadding
			width: parent.width
			anchors.horizontalCenter: parent.horizontalCenter

			onShowDetails: baseItem.pushProviderDetails(pModelItem)
			onShowAdditionalResults: ProviderCategoryFilterModel.addAdditionalResultCategories()
		}
	}
}
