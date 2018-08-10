;; Tuxracer - RGBs saver
;; Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
;;
;; Saves a layed image as seperated RGB images using the layer names
;; as filenames + .rgb
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

(define (script-fu-tuxracer-save-as-rgbs inImage inDraw pathname)
  (let  ((layers (gimp-image-get-layers inImage))
	(layer-count 0))
    
    (while (< layer-count (car layers))
	   (let  ((layer (aref (cadr layers) layer-count)))
	     (file-sgi-save 1 inImage layer
			    (string-append pathname "/"
					   (car (gimp-layer-get-name layer))
					   ".rgb")
			    "foobar"
			    0))
	   (set! layer-count (+ layer-count 1)))))

(script-fu-register
    "script-fu-tuxracer-save-as-rgbs"
    "<Image>/Script-Fu/Tuxracer/Save Image as RGBs"
    "Saves a layed image as seperated RGB images using the layer names as filenames + .rgb"
    "Ingo Ruhnke"
    "1999, Ingo Ruhnke"
    "Fri Mar  3 16:00:13 2000"
    "RGBA"
    SF-IMAGE "Image" 0
    SF-DRAWABLE "Drawable" 0
    SF-VALUE "Directory:" "\"/tmp/\"")
