#Course initialization script
#sets basic course paramaters before the lighting is set.

set cwd [pwd]
tux_goto_data_dir
cd courses/common

source tree_polyhedron.tcl

cd ../textures

tux_load_texture fish items/herring_standard.rgb 0
tux_item_spec -name herring -diameter 1.0 -height 1.0 \
      -texture fish -colour {28 185 204} -above_ground 0.2

tux_load_texture shrub models/shrub.rgb 0
tux_tree_props -name tree3 -diameter 1.4 -height 1.0 \
      -texture shrub -colour {0 255 48} -polyhedron $tree_poly \
      -size_varies 0.5 

tux_load_texture tree models/tree.rgb 0
tux_tree_props -name tree1 -diameter 1.4 -height 2.5 \
      -texture tree -colour {255 255 255} -polyhedron $tree_poly \
      -size_varies 0.5 

tux_load_texture tree_barren models/tree_barren.rgb 0
tux_tree_props -name tree2 -diameter 1.4 -height 2.5 \
      -texture tree_barren -colour {255 96 0} -polyhedron $tree_poly \
      -size_varies 0.5 

tux_load_texture flag1 items/flag.rgb 0
tux_item_spec -name flag -diameter 1.0 -height 1.0 \
      -texture flag1 -colour {194 40 40} -nocollision
      
tux_load_texture finish items/finish.rgb 0
tux_item_spec -name finish -diameter 9.0 -height 6.0 \
		-texture finish -colour {255 255 0} -nocollision \
                -normal {0 0 1}

tux_load_texture start items/start.rgb 0
tux_item_spec -name start -diameter 9.0 -height 6.0 \
		-texture start -colour {128 128 0} -nocollision \
                -normal {0 0 1}

tux_item_spec -name float -nocollision -colour {255 128 255} -reset_point

cd ../common

tux_trees "$cwd/trees.rgb"

cd ../textures

tux_ice_tex terrain/ice.rgb
tux_rock_tex terrain/rock.rgb
tux_snow_tex terrain/snow.rgb

tux_friction 0.22 0.9 0.35

cd ../common

#
# Introductory animation keyframe data
#
source tux_walk.tcl

#
# Lighting
#
set conditions [tux_get_race_conditions]
if { $conditions == "sunny" } {
    source sunny_light.tcl
} elseif { $conditions == "night" } {
    source night_light.tcl
} elseif { $conditions == "evening" } {
    source evening_light.tcl
} 

cd $cwd
