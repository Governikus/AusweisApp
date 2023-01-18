/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
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
		ProviderCategoryFilterModel.setCategorySelection("all");
		ProviderCategoryFilterModel.searchString = "";
		tabbedPane.currentIndex = 0;
	}

	TabbedPane {
		id: tabbedPane
		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		contentDelegate: content
		contentPadding: 0
		sectionsModel: [{
				"categoryName": qsTr("All provider"),
				"category": "all"
			}, {
				"categoryName": qsTr("Citizen services"),
				"category": "citizen"
			}, {
				"categoryName": qsTr("Financials"),
				"category": "finance"
			}, {
				"categoryName": qsTr("Insurances"),
				"category": "insurance"
			}, {
				"categoryName": qsTr("Other services"),
				"category": "other"
			}]

		sectionDelegate: TabbedPaneDelegateIconAndText {
			iconPath: Category.buttonImageSource(model.modelData.category)
			sectionName: model.modelData.categoryName
		}

		onCurrentItemModelChanged: {
			if (currentItemModel === null) {
				return;
			}
			ProviderCategoryFilterModel.setCategorySelection(currentItemModel.modelData.category);
		}

		Component {
			id: content
			ProviderGridView {
				height: tabbedPane.availableHeight
				width: parent.width

				onShowAdditionalResults: {
					ProviderCategoryFilterModel.setCategorySelection("all");
					tabbedPane.currentIndex = 0;
				}
				onShowDetails: pModelItem => {
					baseItem.showDetailView(pModelItem);
				}
			}
		}
	}
}
