import QtQuick 2.6

import "../../../global"


Item {
	id: baseItem

	property string providerCategory
	property int hits: providerModel.categoryFilter.matchesForExcludedCategory(providerCategory)

	Connections {
		target: providerModel.categoryFilter
		onFireCriteriaChanged: {
			hits = providerModel.categoryFilter.matchesForExcludedCategory(providerCategory)
		}
	}

	visible: false

	function selectCategoryIfMatching() {
		if (baseItem.hits > 0) {
			providerModel.categoryFilter.updateCategorySelection(baseItem.providerCategory, true)
		}
	}
}
