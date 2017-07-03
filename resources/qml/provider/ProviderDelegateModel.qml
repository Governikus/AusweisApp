import QtQml.Models 2.2
import QtQuick 2.5

DelegateModel {
	id: root
	property string searchText

	items {
		includeByDefault: true
		onChanged: filterBy(root.searchText)
	}

	groups: DelegateModelGroup {
		name: "filteredItems"
		includeByDefault: false
	}

	filterOnGroup: "filteredItems"

	onSearchTextChanged: {
		filterBy(searchText);
	}

	function match(entry, filterString) {
		var validFilterString = filterString ? filterString : ""

		return entry.model.display.toLowerCase().search(validFilterString.toLowerCase()) !== -1 ||
			entry.model.address.toLowerCase().search(validFilterString.toLowerCase()) !== -1
	}

	function filterBy(filterString) {
		for (var i = 0; i < items.count; i++ ) {
			var entry = items.get(i)

			if (match(entry, filterString)) {
				entry.groups = ["filteredItems", "items"]
				// it's now visible, because we filter on group "filteredItems"
			}
			else {
				entry.groups = ["items"]
			}
		}
	}

}

