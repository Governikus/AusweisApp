/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.HistoryModel 1.0

SectionPage {
	id: baseItem
	enum SubViews {
		None,
		Detail
	}

	isAbstract: true

	titleBarAction: TitleBarAction {
		helpTopic: "provider"
		//: LABEL DESKTOP
		text: qsTr("Provider")

		customSubAction: SearchBar {
			anchors.verticalCenter: parent ? parent.verticalCenter : undefined

			//: LABEL DESKTOP
			placeholderText: qsTr("Search providers")

			onDisplayTextChanged: ProviderCategoryFilterModel.searchString = displayText
		}

		onClicked: {
			d.activeView = ProviderView.SubViews.None;
		}
	}

	Keys.onEscapePressed: event => {
		if (d.activeView === ProviderView.SubViews.None) {
			event.accepted = false;
			return;
		}
		d.activeView = ProviderView.SubViews.None;
	}

	QtObject {
		id: d

		property int activeView: ProviderView.SubViews.None
	}
	ProviderDetailView {
		id: detailView
		visible: d.activeView === ProviderView.SubViews.Detail

		onNextView: pName => {
			d.activeView = ProviderView.SubViews.None;
			baseItem.nextView(pName);
		}
	}
	ProviderOverview {
		id: overviewView
		visible: d.activeView === ProviderView.SubViews.None

		Component.onCompleted: setActive()
		onNextView: pName => {
			baseItem.nextView(pName);
		}
		onShowDetailView: pModelItem => {
			HistoryModel.nameFilter.setProviderAddress(pModelItem.providerAddress);
			detailView.providerModelItem = pModelItem;
			d.activeView = ProviderView.SubViews.Detail;
		}
	}
}
