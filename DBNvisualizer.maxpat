{
	"patcher" : 	{
		"fileversion" : 1,
		"rect" : [ 813.0, 44.0, 633.0, 1272.0 ],
		"bglocked" : 0,
		"defrect" : [ 813.0, 44.0, 633.0, 1272.0 ],
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "multislider",
					"numinlets" : 1,
					"setminmax" : [ 0.0, 6.0 ],
					"patching_rect" : [ 316.0, 77.0, 270.0, 497.0 ],
					"numoutlets" : 2,
					"setstyle" : 5,
					"id" : "obj-6",
					"outlettype" : [ "", "" ],
					"presentation_rect" : [ 316.0, 77.0, 0.0, 0.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "multislider",
					"numinlets" : 1,
					"setminmax" : [ 0.0, 1.0 ],
					"patching_rect" : [ 44.0, 577.0, 537.0, 483.0 ],
					"numoutlets" : 2,
					"id" : "obj-5",
					"outlettype" : [ "", "" ],
					"size" : 100
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "print",
					"numinlets" : 1,
					"patching_rect" : [ 192.0, 56.0, 34.0, 20.0 ],
					"numoutlets" : 0,
					"id" : "obj-4",
					"fontname" : "Arial",
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "route /graph /w /sq",
					"numinlets" : 1,
					"patching_rect" : [ 50.0, 49.0, 110.0, 20.0 ],
					"numoutlets" : 4,
					"id" : "obj-3",
					"fontname" : "Arial",
					"outlettype" : [ "", "", "", "" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "multislider",
					"numinlets" : 1,
					"setminmax" : [ 0.0, 1.0 ],
					"patching_rect" : [ 39.0, 77.0, 270.0, 497.0 ],
					"numoutlets" : 2,
					"setstyle" : 5,
					"id" : "obj-2",
					"outlettype" : [ "", "" ],
					"size" : 7
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "udpreceive 7130",
					"numinlets" : 1,
					"patching_rect" : [ 64.0, 21.0, 99.0, 20.0 ],
					"numoutlets" : 1,
					"id" : "obj-1",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-3", 2 ],
					"destination" : [ "obj-6", 0 ],
					"hidden" : 1,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 0 ],
					"destination" : [ "obj-3", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 0 ],
					"destination" : [ "obj-2", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 1 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 1,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 0 ],
					"destination" : [ "obj-4", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
 ]
	}

}
