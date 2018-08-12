
#
# Procedure to get course name, author, and par time from course.tcl file.
#
proc get_course_info { } {
    if [catch {open course.tcl r} fileId] {
	puts stderr "Couldn't open course.tcl in [pwd]"
	return {}
    }

    set name ""
    set author ""
    set par_time ""

    while {[gets $fileId line] >= 0} {
	regexp {tux_course_name *([^;]*)} $line match name
	regexp {tux_course_author *([^;]*)} $line match author
	regexp {tux_par_time *([^;]*)} $line match par_time

	if { $author != "" && $name != "" && $par_time != "" } {
	    break;
	}
    }

    if { $author == "" } {
	set author "Unknown"
    }

    if { $name == "" } {
	set name "Unknown"
    }

    if { $par_time == "" } {
        set par_time 120.0
    } 

    # Remove quotes around strings, etc.; e.g. "Jasmin Patry" -> Jasmin Patry
    eval "set name $name"
    eval "set author $author"
    eval "set par_time $par_time"

    return [list $name $author $par_time];
}

set cwd [pwd]
tux_goto_data_dir

tux_load_texture noicon textures/noicon.rgb

cd courses

tux_load_texture no_preview textures/hud/nopreview.rgb
tux_bind_texture no_preview no_preview

#
# Bind preview textures if they exist
#
foreach course [glob -nocomplain *] {
    if { $course == "common" || $course == "textures" } {
	continue;
    }
    if [file exists "$course/preview.rgb"] {
	tux_load_texture $course "$course/preview.rgb"
	tux_bind_texture $course $course
    }
}

tux_open_courses [concat \
    { \
	{ \
	    -course frozen_river -name "Frozen River" \
		    -description "Keep your speed down to collect herring !" \
                    -par_time 80.0 \
					-conditions sunny \
	} \
	{ \
	    -course bunny_hill -name "Bunny Hill" \
		    -description "Use clever turning to conquer the Bunny Hill." \
                    -par_time 40.0 \
					-conditions night \
	} \
	{ \
	    -course twisty_slope -name "Twisty Slope" \
		    -description "Tight twists make grabbing herring difficult.  Hard turns will lead you to victory." \
                    -par_time 40.0 \
	} \
	{ \
	    -course bumpy_ride -name "Bumpy Ride" \
		    -description "This hill has a series of ramps tackle.  Make sure to line yourself up before getting airborne." \
                    -par_time 40.0 \
					-conditions sunny \
	} \
     	{ \
	    -course penguins_cant_fly -name "Flying Penguins" \
		    -description "Go fast, and try to keep a bit of control to catch herrings !" \
                    -par_time 120.0 \
	} \
	{ \
	    -course ski_jump -name "Ski Jump" \
		    -description "Jump as far as possible, and be carefull to the landing if you want to catch all the herrings..." \
                    -par_time 40.0 \
					-conditions evening \
	} \
	{ \
	    -course Half_Pipe -name "Half Pipe" \
		    -description "Make tricks shaking the iPhone while jumping to earn points. Be carefull of the time remaining..." \
                    -par_time 80.0 \
					-conditions sunny \
	} \
	{ \
	    -course Off_Piste_Skiing -name "Off piste skiing" \
		    -description "Free-ride in the mountain ! Be careful to trees and use jumps to take shorters ways... Don't forget that stop paddling when speedometers becomes yellow if you want to increase your speed." \
                    -par_time 120.0 \
					-conditions evening \
	} \
	{ \
	    -course in_search_of_vodka -name "In search of vodka" \
		    -description "Tux needs some vodka to warm up is cold belly. Alas, his liquor cabinet has been pillaged. Join Tux on the quest for vodka. Pick up herring for dinner along the way !" \
                    -par_time 120.0 \
					-conditions night \
	} \
	
    } \
 \
]

tux_load_texture herring_run_icon textures/hud/herringrunicon.rgb 0
tux_load_texture cup_icon textures/hud/cupicon.rgb 0

tux_events {
    {
        -name "Trainings" -icon herring_run_icon -cups {
            {
                -name "Turning" -icon cup_icon -races {
                    {
                        -course bunny_hill \
                                -name "Bunny Hill" \
                                -description "Let's start out easy, just collect the herring in the time limit and you'll be fine." \
                                -herring { 15 15 15 15 } \
                                -time { 37 37 37 37 } \
                                -score { 0 0 0 0 } \
                                -mirrored no -conditions night \
                                -windy no -snowing no
                    }
                    {
                       -course twisty_slope \
                                -name "Twisty Slope" \
                                -description "Tight twists make grabbing herring difficult.  Hard turns (turn + brake) will lead you to victory." \
                                -herring { 15 15 15 15 } \
                                -time { 42 42 42 42 } \
                                -score { 0 0 0 0 } \
                                -mirrored no -conditions Sunny \
                                -windy no -snowing no
                    }
                }
             }            
        
              {
               -name "Jumping" -icon cup_icon -races {
					{
                        -course bumpy_ride \
                                -name "Bumpy Ride" \
                                -description "This hill has a series of ramps to tackle.  Be sure to line yourself up before getting airborne." \
                                -herring { 13 13 13 13 } \
                                -time { 33 33 33 33 } \
                                -score { 0 0 0 0 } \
                                -mirrored no -conditions sunny \
                                -windy no -snowing no
                    }
                    {
                        -course Half_Pipe \
                                -name "Half Pipe" \
                                -description "Make tricks shaking the iPhone while jumping to earn points. Be carefull of the time remaining..." \
                                -herring { 0 0 0 0 } \
                                -time { 0 0 0 0 } \
                                -score { 2500 2500 2500 2500 } \
                                -mirrored no -conditions sunny \
                                -windy no -snowing no
                    }
              }
	      
	     	}
	
			{
               -name "Controlling speed" -icon cup_icon -races {
                    {
                        -course Off_Piste_Skiing \
                                -name "Off piste skiing" \
                                -description "Free-ride in the mountain ! Be careful to trees and use jumps to take shorters ways... Don't forget that stop paddling when speedometers becomes yellow if you want to increase your speed." \
                                -herring { 12 12 12 12 } \
                                -time { 120 120 120 120 } \
                                -score { 0 0 0 0 } \
                                -mirrored no -conditions evening \
                                -windy no -snowing no
                    }
					{
                        -course frozen_river \
                                -name "Frozen River" \
                                -description "Keep your speed down to collect herring !" \
                                -herring { 30 30 30 30 } \
                                -time { 110 110 110 110 } \
                                -score { 0 0 0 0 } \
                                -mirrored no -conditions sunny \
                                -windy no -snowing no
                    }
              }
	      
	     	}
		}
	}	
}
    

cd $cwd

