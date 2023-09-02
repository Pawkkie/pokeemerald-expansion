EventScript_TypeChart::
	lock
	setvar VAR_0x8000, MUGSHOT_TYPE_CHART
	callnative DrawMugshot
	waitbuttonpress
	callnative ClearMugshot
	release
    end
    
