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

uniform vec4 skale;
uniform vec4 kolor;
uniform vec4 ankle;
uniform vec4 kamera;
uniform vec4 kenter;
uniform vec4 greek_x;
uniform vec4 greek_y;
uniform vec4 greek_z;
uniform vec4 prekalkulated_x;
uniform vec4 prekalkulated_y;
uniform vec4 prekalkulated_z;
uniform vec4 windowsize;
uniform vec4 stepwidth;

varying vec3 bunch_position;

//we assume for now, we are in the fragment shader
#ifdef OLAV_DOVOLUME

#ifdef OLAV_ANAL
float erf_positive(float x)
{
	float devisor = 1.0+0.278393*x+0.230389*x*x+0.000972*x*x*x+0.078108*x*x*x*x;
	devisor *= devisor; //²
	devisor *= devisor; //²
	return 1.0-1.0/devisor;
}

float erf(float x)
{
	if (x < 0.0) 
		return -erf_positive(-x);
	return erf_positive(x);
}

float gauss(float x,vec4 greek,vec4 prekalkulated)
{
	float alpha = greek.x;
	float beta = greek.y;
	//float gamma = greek.z;
	float epsilon = greek.w;
	float biggest = greek.z;
	float k = prekalkulated.x;
	float l = prekalkulated.y;
	float m = prekalkulated.z;
	float n = prekalkulated.w;
	float part = sqrt(n*(beta*epsilon-x*x))/beta;
	#ifdef OLAV_APPROX
		float radius = 2.0;
		return abs(k*exp(l*x*x*radius*radius)*(erf(m*radius*x+part)-erf(m*radius*x-part)))/biggest;
	#else
		return abs(k*exp(l*x*x)*(erf(m*x+part)-erf(m*x-part)))/biggest;
	#endif
}
#endif

uniform sampler3D volume_texture;
uniform sampler2D transfer_texture;
uniform sampler2D rtt_texture;

vec4 trace_volume(vec3 start_position,vec3 end_position,vec4 start_color,vec4 scale)
{
	float alpha = start_color.a;
	vec4 color = vec4(start_color.r,start_color.g,start_color.b,0.0);

	vec3 sensor = start_position;

	vec3 vector = (end_position-start_position);
	float l = length(vector);
	float stepwidth = 0.001;
	int count = int(l/stepwidth);
	vector/=float(count);
	
	if (count < 1)
		return vec4(1.0,1.0,1.0,0.0);
	if (count > 1000)
		count = 1000;
	for (int i = 0; i < count && color.a < 0.99; i++)
	{
		#ifdef OLAV_FINE
		vec4 t_color = texture3D(volume_texture, (sensor+1.0)/2.0);
		float value = t_color.r * 256.0
					+ t_color.g
					+ t_color.b / 256.0
					+ t_color.a / 65536.0;
		vec4 transfer = texture2D(transfer_texture, vec2(value*2.0,0.9));
		vec3 colorSample = transfer.rgb;
		float alphaSample = transfer.a*stepwidth*50.0;
		vec4 blob = vec4(((1.0-color.a)*colorSample*alphaSample),alphaSample);
		#endif
		#ifdef OLAV_RAW
		vec4 t_color = texture3D(volume_texture, (sensor+1.0)/2.0);
		float value_1 = t_color.r * 256.0
					+ t_color.g
					+ t_color.b / 256.0
					+ t_color.a / 65536.0;
			 t_color = texture3D(volume_texture, (sensor+vector+1.0)/2.0);
		float value_2 = t_color.r * 256.0
					+ t_color.g
					+ t_color.b / 256.0
					+ t_color.a / 65536.0;
		vec4 transfer = texture2D(transfer_texture, vec2(value_1,value_2));
		vec3 colorSample = transfer.rgb;
		float alphaSample = transfer.a*50.0*0.001;//*stepwidth
		vec4 blob = vec4(((1.0-color.a)*colorSample*alphaSample),alphaSample);
		#endif
		#ifdef OLAV_ANAL
		float value = gauss(sensor.x*skale.x,greek_x,prekalkulated_x)
					* gauss(sensor.y*skale.y,greek_y,prekalkulated_y)
					* gauss(sensor.z*skale.z,greek_z,prekalkulated_z)/300.0;
		vec4 transfer = texture2D(transfer_texture, vec2(value*2.0,0.9));
		vec3 colorSample = transfer.rgb;
		float alphaSample = transfer.a*stepwidth*50.0;
		vec4 blob = vec4(((1.0-color.a)*colorSample*alphaSample),alphaSample);
		#endif
		color += blob;
		sensor += vector;
	}
	if (kamera.w == 1.0)
		color.a *= alpha;
	return color;
}
#endif

#ifdef OLAV_VERTEX
void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	#ifdef OLAV_FINE
		bunch_position.x = gl_Vertex.x/5.01;
		bunch_position.y = gl_Vertex.y/5.01;
		bunch_position.z = gl_Vertex.z/5.01;
	#endif
	#ifdef OLAV_RAW
		bunch_position.x = gl_Vertex.x/5.01*skale.x/kenter.w;
		bunch_position.y = gl_Vertex.y/5.01*skale.y/kenter.w;
		bunch_position.z = gl_Vertex.z/5.01*skale.z/kenter.w;
	#endif
	#ifdef OLAV_ANAL
		bunch_position.x = gl_Vertex.x/5.01;
		bunch_position.y = gl_Vertex.y/5.01;
		bunch_position.z = gl_Vertex.z/5.01;
	#endif
}
#endif

#ifdef OLAV_FRAGMENT
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
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
	else
	{
		vec3 rtt_vertex = (rtt_color*2.0-vec3(1.0,1.0,1.0))*5.01;
		vec3 bunch_end;
		#ifdef OLAV_FINE
			bunch_end.x = rtt_vertex.x/5.01;
			bunch_end.y = rtt_vertex.y/5.01;
			bunch_end.z = rtt_vertex.z/5.01;
		#endif
		#ifdef OLAV_RAW
			bunch_end.x = rtt_vertex.x/5.01*skale.x/kenter.w;
			bunch_end.y = rtt_vertex.y/5.01*skale.y/kenter.w;
			bunch_end.z = rtt_vertex.z/5.01*skale.z/kenter.w;
		#endif
		#ifdef OLAV_ANAL
			bunch_end.x = rtt_vertex.x/5.01;
			bunch_end.y = rtt_vertex.y/5.01;
			bunch_end.z = rtt_vertex.z/5.01;
		#endif
		if (kolor.w == 0.0f)
			gl_FragColor = vec4(0.0,0.0,0.0,0.0);
		else
			gl_FragColor = trace_volume(bunch_position,bunch_end,kolor,skale);
	}
}
#endif

#ifdef OLAV_VERTEX_FIRST
void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = vec4((((gl_Vertex.xyz/5.01)+vec3(1.0,1.0,1.0))/2.0).xyz,1.0);
}
#endif

#ifdef OLAV_FRAGMENT_FIRST
void main(void)
{
	gl_FragColor = gl_Color;
}
#endif
