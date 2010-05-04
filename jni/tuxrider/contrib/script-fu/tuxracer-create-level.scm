;; Tuxracer - Creates a framework for a new level
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

(define (script-fu-tuxracer-create-new-level width height)
  (let ((image (car (gimp-image-new width height RGB))))
    (let ((layer_elev    (car (gimp-layer-new image width height RGB "elev" 100 0)))
	  (layer_trees   (car (gimp-layer-new image width height RGB "trees" 100 0)))
	  (layer_terrain (car (gimp-layer-new image width height RGB "terrain" 100 0))))

      (gimp-palette-set-background '(150 150 150))
      (gimp-drawable-fill layer_elev    BG-IMAGE-FILL)
      
      (gimp-palette-set-background '(0 0 0))
      (gimp-drawable-fill layer_trees   BG-IMAGE-FILL)

      (gimp-palette-set-background '(255 255 255))
      (gimp-drawable-fill layer_terrain BG-IMAGE-FILL)

      (gimp-image-add-layer image layer_trees 0)
      (gimp-image-add-layer image layer_terrain 0)
      (gimp-image-add-layer image layer_elev 0)
      
      (gimp-display-new image)
      )))

(script-fu-register "script-fu-tuxracer-create-new-level"
		    "<Toolbox>/Xtns/Script-Fu/TuxRacer/New Level..."
		    "Creates a new tuxracer level"
		    "Ingo Ruhnke"
		    "1999, Ingo Ruhnke"
		    "2000"
		    "RGB RGBA"
		    SF-VALUE "Width:" "40"
		    SF-VALUE "Height:" "300")

;; EOF
