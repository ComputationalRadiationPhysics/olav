// simple geometry shader

// these lines enable the geometry shader support.
#version 150 compatibility
#extension GL_EXT_geometry_shader4 : enable


layout (points) in;
layout (line_strip,max_vertices = 90) out;

in VertexAttrib
{
  vec3 normal;
  vec4 color;
} vertex[];

//out vec3 ViewDirection;
uniform vec4 LightPosition;
out vec3 Normal;
uniform vec4 cameraPosition;
uniform float time;
//uniform float maxlength;
//uniform float resolution;
uniform float speed;
uniform float radius;
float step=0.2;


void main( void )
{


float min_phi=vertex[0].normal.x;
float max_phi=vertex[0].normal.y;
float resolution=vertex[0].normal.z;
float min_theta=gl_PositionIn[0].x;
float max_theta=gl_PositionIn[0].y;
float maxlength=gl_PositionIn[0].z;



vec4 delta;
float l=0.0;
float start=time-(maxlength*step);
float s=0.0;
float x=0.0;
int i=0;
vec4 lastpos,pos;
float teta=0.0;
vec4 dir;


//Trajektorie
for (float t=time;(t>time-0.2) && (t>0.0);t-=0.1){
float s=speed*t;
gl_FrontColor  = vec4(1.0-(time-t),1.0-(time-t),1.0-(time-t),1.0);
pos=vec4(0.0,sin(s/radius)*radius,0.0,1.0);
gl_Position=gl_ModelViewProjectionMatrix*(vec4(2.0,2.0,2.0,1.0)*pos);
Normal=cross(lastpos.xyz-pos.xyz,cross(pos.xyz-gl_LightSource[0].position.xyz,lastpos.xyz-pos.xyz));
lastpos=pos;
EmitVertex();
}
EndPrimitive();

//Feldlinien
for (float t=min_theta;t<max_theta;t+=resolution){
teta=sin(t);
for (float p=min_phi;p<max_phi;p+=resolution){
dir=vec4(teta*cos(p),teta*sin(p),cos(t),0.0);
	for (float ti=time;(ti>=start) /*&& (t>0.0)*/;ti-=step){
	s=speed*(start+time-ti);
	x=sin(s/radius)*radius;
	delta=vec4(0.0,sin(speed*(start+time-ti+step)/radius)*radius,0.0,1.0)-vec4(0.0,x,0.0,1.0);
	l=length(delta);
	//gl_FrontColor  = vec4(1.0,1.0,1.0,1.0);
	gl_FrontColor  = vertex[0].color*(time-ti);//vec4((time-ti),(time-ti),(time-ti),1.0);
	pos=(vec4(0.0,x,0.0,1.0)
				+(normalize(dir*step+
					delta)
			)
			*(ti-start));
	gl_Position=gl_ModelViewProjectionMatrix*(vec4(2.0,2.0,2.0,1.0)*pos);

	Normal=cross(lastpos.xyz-pos.xyz,cross(pos.xyz-LightPosition.xyz,lastpos.xyz-pos.xyz));
	lastpos=pos;
	i++;
	EmitVertex();
	}
	EndPrimitive();
}
}
}