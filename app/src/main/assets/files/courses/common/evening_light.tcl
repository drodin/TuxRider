tux_course_light 0 -on -position { 1 1 0 0 } \
	-diffuse {0.7 0.7 0.7 1.0} \
	-ambient {0.8 0.55 0.5 1.0} 
  
tux_course_light 1 -on -position { 1 1 2 0 } \
	-specular { 0.9 0.55 0.5 1 } 

tux_fog -on -mode linear -density 0.005 -colour { 0.45 0.3 0.15 1 } -start 0 \
    -end [tux_get_param forward_clip_distance]

tux_particle_color { 0.9 0.7 0.35 1.0 }

#
# Environmental sphere map
    
tux_load_texture alpine1-sphere ../textures/conditions/eveningenv.rgb 0
tux_bind_texture terrain_envmap alpine1-sphere

tux_particle_colour { 0.51 0.30 0.30 1.0 }

tux_load_texture alpine1-front ../textures/conditions/eveningfront.rgb 0
tux_load_texture alpine1-right ../textures/conditions/eveningright.rgb 0
tux_load_texture alpine1-left ../textures/conditions/eveningleft.rgb 0
tux_load_texture alpine1-back ../textures/conditions/eveningback.rgb 0
tux_load_texture alpine1-top ../textures/conditions/eveningtop.rgb 0
tux_load_texture alpine1-bottom ../textures/conditions/eveningbottom.rgb 0

tux_bind_texture sky_front alpine1-front
tux_bind_texture sky_right alpine1-right
tux_bind_texture sky_left alpine1-left
tux_bind_texture sky_back alpine1-back
tux_bind_texture sky_top alpine1-top
tux_bind_texture sky_bottom alpine1-bottom