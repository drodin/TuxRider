#
# Course configuration
#
tux_course_name "Half Pipe"
tux_course_author "Emmanuel de Roux"
tux_course_dim 40 1500       ;# width, length of course in m
tux_start_pt 19.5 3.0           ;# start position, measured from left rear corner
tux_angle  37.5                 ;# angle of course
tux_elev_scale 15.0             ;# amount by which to scale elevation data
tux_base_height_value 255     ;# greyscale value corresponding to height
                               ;#     offset of 0 (integer from 0 - 255)
tux_elev elev.rgb              ;# bitmap specifying course elevations
tux_terrain terrain.rgb        ;# bitmap specifying terrains type
tux_calculation_mode Half_Pipe      ;# optional : this indicates if the calculation mode


tux_course_init
