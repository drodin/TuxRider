;; Tuxracer - Load all files of a course into a single image
;; Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;; 
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;; 
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

(define (load-image pathname filename-raw)
  (let ((filename (string-append pathname filename-raw)))
    (car (file-sgi-load 0 filename filename))))

(define (script-fu-tuxracer-load-level pathname)
  (let ((image_elev    (load-image pathname "/elev.rgb"))
	(image_trees   (load-image pathname "/trees.rgb"))
	(image_terrain (load-image pathname "/terrain.rgb")))
    
    (let ((width  (car (gimp-image-width image_elev)))
	  (height (car (gimp-image-height image_elev))))
      (let ((new-image (car (gimp-image-new width height RGB))))
	(let ((layer_elev    (car (gimp-layer-new new-image width height RGB "elev" 100 0)))
	      (layer_trees   (car (gimp-layer-new new-image width height RGB "trees" 100 0)))
	      (layer_terrain (car (gimp-layer-new new-image width height RGB "terrain" 100 0))))

	  (gimp-image-add-layer new-image layer_trees 0)
	  (gimp-image-add-layer new-image layer_terrain 0)
	  (gimp-image-add-layer new-image layer_elev 0)

	  (gimp-layer-add-alpha layer_trees)
	  (gimp-layer-add-alpha layer_terrain)
	  (gimp-layer-add-alpha layer_elev)

	  (gimp-edit-copy (aref (cadr (gimp-image-get-layers image_elev)) 0))
	  (gimp-floating-sel-anchor (car (gimp-edit-paste layer_elev 0)))

	  (gimp-edit-copy (aref (cadr (gimp-image-get-layers image_trees)) 0))
	  (gimp-floating-sel-anchor (car (gimp-edit-paste layer_trees 0)))

	  (gimp-edit-copy (aref (cadr (gimp-image-get-layers image_terrain)) 0))
	  (gimp-floating-sel-anchor (car (gimp-edit-paste layer_terrain 0)))
	  
	  (gimp-display-new new-image)

	  (gimp-image-delete image_elev)
	  (gimp-image-delete image_terrain)
	  (gimp-image-delete image_trees)

	  (gimp-displays-flush))))
      ))
  
(script-fu-register "script-fu-tuxracer-load-level"
		    "<Toolbox>/Xtns/Script-Fu/TuxRacer/Load Level..."
		    "Tuxracer - Load all files of a course into a single image"
		    "Ingo Ruhnke"
		    "2000, Ingo Ruhnke"
		    "2000"
		    "RGB RGBA"
		    SF-FILENAME "Directory:" "/home")

;; EOF ;;
