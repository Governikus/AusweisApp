/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: baseItem

	signal showDetailView(var pModelItem)

	Component.onCompleted: {
		ProviderCategoryFilterModel.setCategorySelection("all")
		ProviderCategoryFilterModel.searchString = ""
		tabbedPane.currentIndex = 0
	}

	TabbedPane {
		id: tabbedPane

		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		sectionsModel: [
			{
				//: LABEL DESKTOP
				categoryName: qsTr("All provider"),
				category: "all"
			},
			{
				//: LABEL DESKTOP
				categoryName: qsTr("Citizen services"),
				category: "citizen"
			},
			{
				//: LABEL DESKTOP
				categoryName: qsTr("Financials"),
				category: "finance"
			},
			{
				//: LABEL DESKTOP
				categoryName: qsTr("Insurances"),
				category: "insurance"
			},
			{
				//: LABEL DESKTOP
				categoryName: qsTr("Other services"),
				category: "other"
			}
		]

		sectionDelegate: TabbedPaneDelegateIconAndText {
			sectionName: model.modelData.categoryName
			iconPath: Category.buttonImageSource(model.modelData.category)
		}
		contentDelegate: content
		contentPadding: 0
		onCurrentItemModelChanged: {
			if (currentItemModel === null) {
				return
			}

			ProviderCategoryFilterModel.setCategorySelection(currentItemModel.modelData.category)
		}

		Component {
			id: content

			ProviderGridView {
				height: tabbedPane.availableHeight
				width: parent.width

				onShowDetails: pModelItem => { baseItem.showDetailView(pModelItem) }
				onShowAdditionalResults: {
					ProviderCategoryFilterModel.setCategorySelection("all")
					tabbedPane.currentIndex = 0
				}
			}
		}
	}
}
