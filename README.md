# PhotonMapping
- Author: zy-li14@hotmail.com
- This is a photo render program using photon-mapping. 
- I learn the algorithm from *realistic image synthesis using photon mapping*.
- I use Qt's QImage to read and write pictures, so Qt runtime enviroment is essential.

## Usage:
``` bash
photonMapping.exe scene.txt ret.jpg
```
The format of input scene file is as simple as the following. More examples is in the root directory of this project (*.txt files)
```
/*
I am block comment
*/
# I am line comment

#I am the valid range of the scene
leftBottom: -5.001 -5.001 -5.001
rightTop: 5.001 5.001 5.001
#How many threads can I use
maxThreadNum: 30
start camera
    size: 1 1
    dx: 0 -1 0
    dy: -0.3 0 1
    center: 3 0.3 2.5
    normal: -1 0 -0.3
    focus: 1
    dpi: 200
    radius: 0.00001
    environment: 0.01 0.01 0.01
    K: 100
    brightnessValue: 10000 # |global photons| = |light| * brightnessValue
    raysPerPixel: 1 #How many rays will I use to render one pixel.
    causticScale: 1 # |caustic photons| = |global photons| * causticScale
end
start light
	type: point
	center: 3.5 1 6
	color: 1 1 1
end
/* 
start light
    type: rectangle
    center: 1.4 1.5 5
    size: 0.2 0.2
    normal: 0 0 -1
    color: 1 1 1
    dx: 1 0 0
    dy: 0 1 0
    quality: 50
end
*/
#==============================================================
# material
# Declare me before objects
#==============================================================
start material #stone
	color: 1 1 1
	absorb: 0 0 0
	texture: ./floor.bmp
    diffusion: 1
	reflection: 0
	refraction: 0
	refractivity: 1.0
end

start material #clear without red
	color: 1 1 1
	absorb: 1 0 0
	diffusion: 0
	reflection: 0
	refraction: 1
	refractivity: 2.54
end

start material #clear without green
	color: 1 1 1
	absorb: 0 1 0
	diffusion: 0
	reflection: 0
	refraction: 1
	refractivity: 2.54
end

start material #clear without blue
	color: 1 1 1
	absorb: 0 0 1
	diffusion: 0
	reflection: 0
	refraction: 1
	refractivity: 2.54
end

start material #gold rank4
	color: 0.9804 0.84375 0.375
	absorb: 0.1196 0.15625 0.625
	diffusion: 1
	reflection: 0
	refraction: 0
	refractivity: 1.4
end

start material #glass
	color: 0.9804 0.84375 0.375
	absorb: 0.453 0.617 0.78
	diffusion: 0.0
	reflection: 0.0
	refraction: 1
	refractivity: 1.4
end
#==============================================================
# object
#==============================================================
start object
	type: plane
	center: -8 -2 0
	normal : 0 0 1
	material: 0
	dx: 0 12 0
	dy: 10 0 0
end
/*
start model
	model: ./models/fixed.perfect.dragon.100K.0.07.obj
	center: 1.0 1.5 1.0
	size: 1
	rotate: 90 0 90
	material: 4
end
*/

start model
    model: ./models/horse.fine.90k.obj
    center: 1 1.5 1.0
    size: 1
    rotate: 0 0 180
    material: 5
end
/*
start model
	model: ./models/Cup.obj
	center: 1.0 1.5 1.0
	size: 0.02
	rotate: 0 0 60
	material: 5
end

*/
/*
start model
	model: ./models/Buddha.obj
	center: 1.0 1.5 1
	size: 1
	rotate: 90 0 90
	material: 4
end
start model
	model: ./models/block.obj
	center: 0.5 0.5 0.5
	size: 0.03
	rotate: 90 0 90
	material: 4
end
start model
	model: ./models/cube.obj
	center: 0.5 0.5 0.5
	size: 0.8
	rotate: 90 0 90
	material: 4
end
*/

```