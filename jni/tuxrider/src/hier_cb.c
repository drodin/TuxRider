/* 
 * Tux Racer 
 * Copyright (C) 1999-2001 Jasmin F. Patry
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include "tuxracer.h"
#include "hier.h"
#include "tcl_util.h"

/*
 * Callbacks
 */

int tux_rotate( ClientData cd, Tcl_Interp *ip, int argc, char *argv[] ) 
{
    char *errmsg;

    char *nodename;
    char axis;
    double angle ;

    if (4 != argc) {
        Tcl_AppendResult(ip, argv[0], ": wrong number of arguments\n", 
			 "Usage: ", argv[0], " <node> [x|y|z] <angle>",
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain the nodename */
    nodename = argv[1];

    /* obtain the axis */
    axis = argv[2][0];
    if ('x' != axis && 'y' != axis && 'z' != axis) {
        Tcl_AppendResult(ip, argv[0], ": invalid rotation axes", 
			 (char *)0 );
        return TCL_ERROR;
    }
    
    /* obtain the angle */
    if (TCL_OK != Tcl_GetDouble(ip, argv[3], &angle)) {
        Tcl_AppendResult(ip, argv[0], ": invalid rotation angle", 
			 (char *)0 );
        return TCL_ERROR;
    }
    
    errmsg = rotate_scene_node(nodename,axis,angle);

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
  
    return TCL_OK;
}

int tux_translate( ClientData cd, Tcl_Interp *ip, int argc, char *argv[] ) 
{
    char *errmsg;

    char *nodename;
    scalar_t vec[3];

    if (3 != argc) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <node> { <x> <y> <z> }",
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain the nodename */
    nodename = argv[1];

    /* obtain the translation vector */
    if (TCL_OK != get_tcl_tuple(ip,argv[2],vec,3)) {
        Tcl_AppendResult(ip, argv[0], ": invalid translation vector", 
			 (char *)0 );
        return TCL_ERROR;
    }
    
    errmsg = translate_scene_node(nodename,make_vector_from_array(vec));

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
  
    return TCL_OK;
}

int tux_scale( ClientData cd, Tcl_Interp *ip, int argc, char *argv[] ) 
{
    char *errmsg;

    char *nodename;
    scalar_t origin[3];
    scalar_t factors[3]; 

    if (4 != argc) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <node> { <origin> } "
			 "{ <translation vector> }",
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain the nodename */
    nodename = argv[1];

    /* obtain the origin point */
    if (TCL_OK != get_tcl_tuple(ip,argv[2],origin,3)) {
        Tcl_AppendResult(ip, argv[0], ": invalid origin point", 
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain the scale factors */
    if (TCL_OK != get_tcl_tuple(ip,argv[3],factors,3)) {
        Tcl_AppendResult(ip, argv[0], ": invalid scale factors", 
			 (char *)0 );
        return TCL_ERROR;
    }
    
    errmsg = scale_scene_node(nodename,make_point_from_array(origin),factors);

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
  
    return TCL_OK;
}

int tux_transform( ClientData cd, Tcl_Interp *ip, int argc, char *argv[] ) 
{
    char *errmsg;

    char *parent_name;
    char *child_name;

    if (3 != argc) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <parent node> <child node>",
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain parent's name */
    parent_name = argv[1];

    /* obtain child's name */
    child_name  = argv[2];

    errmsg = create_tranform_node(parent_name, child_name);

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
    return TCL_OK;
}

int tux_sphere( ClientData cd, Tcl_Interp *ip, int argc, char *argv[] ) 
{
    char *errmsg;

    char *parent_name;
    char *child_name;
    double resolution;

    if (4 != argc) {
	Tcl_AppendResult( ip, argv[0], ": wrong number of arguments\n",
			 "Usage: ", argv[0], " <parent node> <child node> "
			  "<radius>",
			  (char*) 0 );
        return TCL_ERROR;
    }

    /* obtain parent's name */
    parent_name = argv[1];

    /* obtain child's name */
    child_name  = argv[2];

    if ( TCL_OK != Tcl_GetDouble( ip, argv[3], &resolution ) ) {
	Tcl_AppendResult( ip, argv[0], ": resolution is invalid",
			  (char*) 0 );
	return TCL_ERROR;
    }

    errmsg = create_sphere_node(parent_name, child_name, resolution);

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
    return TCL_OK;
}


int tux_material( ClientData cd, Tcl_Interp *ip, int argc, char *argv[] ) 
{
    char *errmsg;

    char *mat_name;
    scalar_t diffuse[3];
    scalar_t specular[3];
    double spec_exp;

    if (5 != argc) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <name> { <ambient colour> } "
			 "{ <specular colour> } <specular exponent",
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain material name */
    mat_name = argv[1];

    /* obtain diffuse colour */
    if (TCL_OK != get_tcl_tuple(ip,argv[2],diffuse,3)) {
        Tcl_AppendResult(ip, argv[0], ": invalid diffuse colour", 
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain specular colour */
    if (TCL_OK != get_tcl_tuple(ip,argv[3],specular,3)) {
        Tcl_AppendResult(ip, argv[0], ": invalid specular colour", 
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain specular exponent */
    if (TCL_OK != Tcl_GetDouble(ip,argv[4],&spec_exp)) {
        Tcl_AppendResult(ip, argv[0], ": invalid specular exponent", 
			 (char *)0 );
        return TCL_ERROR;
    }

    errmsg = create_material(mat_name,make_colour_from_array(diffuse),
                make_colour_from_array(specular), spec_exp);

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
    return TCL_OK;
}

int tux_surfaceproperty( ClientData cd, Tcl_Interp *ip, 
			 int argc, char *argv[] ) 
{
    char *errmsg;

    char *node_name;
    char *mat_name;

    if (3 != argc) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <node> <material name>",
			 (char *)0 );
        return TCL_ERROR;
    }

    /* obtain node name */
    node_name = argv[1];

    /* obtain material name */
    mat_name  = argv[2];

    errmsg = set_scene_node_material(node_name, mat_name);

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
    return TCL_OK;
}

int tux_shadow( ClientData cd, Tcl_Interp *ip, int argc, char *argv[] ) 
{
    char *errmsg;

    char *node_name;
    char *state;

    if (3 != argc) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <node> [on|off]",
			 (char *)0 );
        return TCL_ERROR;
    }

    node_name = argv[1];
    state = argv[2];

    errmsg = set_scene_node_shadow_state(node_name, state);

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
    return TCL_OK;
}

int tux_eye( ClientData cd, Tcl_Interp *ip, int argc, char *argv[] ) 
{
    char *errmsg;

    char *node_name;
    char *which_eye;

    if (3 != argc) {
        Tcl_AppendResult(ip, argv[0], ": invalid number of arguments\n", 
			 "Usage: ", argv[0], " <node> [left|right]",
			 (char *)0 );
        return TCL_ERROR;
    }

    node_name = argv[1];
    which_eye = argv[2];

    errmsg = set_scene_node_eye( node_name, which_eye );

    /* report error, if any */
    if (errmsg) {
        Tcl_AppendResult(ip, argv[0], ": ", errmsg, (char *)0 );
        return TCL_ERROR;
    }
    return TCL_OK;
}

int registerHierCallbacks (
    Tcl_Interp *ip
) {
    Tcl_CreateCommand(ip, "tux_rotate",          tux_rotate,          0,0);
    Tcl_CreateCommand(ip, "tux_translate",       tux_translate,       0,0);
    Tcl_CreateCommand(ip, "tux_scale",           tux_scale,           0,0);
    Tcl_CreateCommand(ip, "tux_sphere",          tux_sphere,          0,0);
    Tcl_CreateCommand(ip, "tux_transform",       tux_transform,       0,0);
    Tcl_CreateCommand(ip, "tux_material",        tux_material,        0,0);
    Tcl_CreateCommand(ip, "tux_surfaceproperty", tux_surfaceproperty, 0,0);
    Tcl_CreateCommand(ip, "tux_shadow",          tux_shadow,          0,0);
    Tcl_CreateCommand(ip, "tux_eye",             tux_eye,             0,0);

    return TCL_OK;
}
