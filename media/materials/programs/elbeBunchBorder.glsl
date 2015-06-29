/**
* Copyright 2015 Alexander Matthes
*
* This file is part of OLAV.
*
* OLAV is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* OLAV is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OLAV.
* If not, see <http://www.gnu.org/licenses/>.
*/

#version 120

uniform vec4 windowsize;

#ifdef OLAV_VERTEX
void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
#endif

#ifdef OLAV_FRAGMENT
uniform sampler2D rtt_texture;
void main(void)
{
	//Calculating the Ending glVertex from the rtt_texture
	float x = (gl_FragCoord.x-windowsize.x)/windowsize.z+0.5;
	float y = (gl_FragCoord.y-windowsize.y)/windowsize.w+0.5;
	#ifdef OLAV_HACKY
		vec3 rtt_color = texture2D(rtt_texture, vec2(x,y)).rgb;
	#else
		vec3 rtt_color = texture2D(rtt_texture, vec2(x,1.0-y)).rgb;
	#endif
	if (rtt_color == vec3(0.0,0.0,0.0)) //black
		gl_FragColor = vec4(0.0,0.3,0.5,0.3);
	else
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
	//gl_FragColor = vec4(rtt_color,1.0);
}
#endif
