/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

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
		None = 0,
		Detail
	}

	Keys.onEscapePressed: {
		if (d.activeView === ProviderView.SubViews.None) {
			event.accepted = false
			return
		}

		d.activeView = ProviderView.SubViews.None
	}

	isAbstract: true

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Provider")
		helpTopic: "provider"

		onClicked: {
			d.activeView = ProviderView.SubViews.None
		}

		customSubAction: SearchBar {
			anchors.verticalCenter: parent ? parent.verticalCenter : undefined

			onDisplayTextChanged: ProviderCategoryFilterModel.searchString = displayText

			//: LABEL DESKTOP
			placeholderText: qsTr("Search providers")
		}
	}

	QtObject {
		id: d

		property int activeView: ProviderView.SubViews.None
	}

	ProviderDetailView {
		id: detailView

		visible: d.activeView === ProviderView.SubViews.Detail

		activeFocusOnTab: true

		onNextView: {
			d.activeView = ProviderView.SubViews.None
			baseItem.nextView(pName)
		}
	}

	ProviderOverview {
		id: overviewView

		visible: d.activeView === ProviderView.SubViews.None

		Component.onCompleted: setActive()

		activeFocusOnTab: true

		onNextView: baseItem.nextView(pName)
		onShowDetailView: {
			HistoryModel.nameFilter.setProviderAddress(pModelItem.providerAddress)
			detailView.providerModelItem = pModelItem
			d.activeView = ProviderView.SubViews.Detail
		}
	}
}
