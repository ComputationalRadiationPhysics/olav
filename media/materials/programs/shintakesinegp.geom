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
float step2=0.001;

//float timescale=0.33;

void main( void )
{

float periode=8.0;
float offset=0.0;
int lock=0;
float pway=0.0;
float way=0.0;

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
float y=0.0;
float alpha=0.0;
int i=0;
vec4 firstpos;
int wtf=1;
vec4 lastpos=vec4(0.0,0.0,0.0,1.0);
vec4 help;
vec4 pos;

float teta=0.0;
vec4 dir;

const mat4 scalemat=mat4(	-2.0, 0.0, 0.0, 0.0,
				0.0, 2.0, 0.0, 0.0,
				0.0, 0.0, 2.0, 0.0,
				0.0, 0.0, 0.0, 1.0);
				
//Trajektorie
for (float t=time;(t>time-step*2.0) && (t>0.0);t-=step){
//x=180.0/3.14*x;
alpha=radius*cos(2.0*3.14/periode*x)*2.0*3.14/periode;
gl_FrontColor  = vec4(1.0,1.0,1.0,1.0);
help=lastpos-speed*step*normalize(vec4(1.0,alpha,0.0,1.0));
pos=vec4(help.x,radius*step*sin((help.x)*2.0*3.14/periode),0.0,1.0);

gl_Position=gl_ModelViewProjectionMatrix*(pos);
Normal=cross(lastpos.xyz-pos.xyz,cross(pos.xyz-gl_LightSource[0].position.xyz,lastpos.xyz-pos.xyz));
lastpos=pos;
x=pos.x;
EmitVertex();
}
EndPrimitive();

//Debug start
/*gl_FrontColor  = vec4(0.0,1.0,0.0,1.0);
gl_Position=gl_ModelViewProjectionMatrix*vec4(0.0,5.0,0.0,1.0);
EmitVertex();
gl_Position=gl_ModelViewProjectionMatrix*vec4(10.0,5.0,0.0,1.0);
EmitVertex();
EndPrimitive();*/
//Debug end


//Werte zurücksetzen


x=0.0;
y=0.0;
way=0.0;
while (x<=(periode/4.0)){
	x+=step2;//ElectronPosition.x
	way+=sqrt(pow(step2,2.0)+pow(radius*sin(x*2.0*3.14/periode)-y,2.0));
	y=radius*sin(x*2.0*3.14/periode);//ElectronPosition.y
	if (lock!=1 && x>=(periode/4.0-step2)) {
							offset=x;
							pway=way;
							lock=1;
		//break;
	}
}

x=0.0;
y=0.0;
way=0.0;

//Debug start
/*gl_FrontColor  = vec4(1.0,0.0,0.0,1.0);
gl_Position=gl_ModelViewProjectionMatrix*vec4(0.0,0.0,0.0,1.0);
EmitVertex();
gl_Position=gl_ModelViewProjectionMatrix*vec4(10.0,0.0,0.0,1.0);
EmitVertex();
EndPrimitive();*/
//Debug end



//Feldlinien
for (float t=min_theta;t<max_theta;t+=resolution){
teta=sin(t);
for (float p=min_phi;p<max_phi;p+=resolution){

dir=vec4(teta*cos(p),teta*sin(p),cos(t),0.0);//k0 in ruhe


	dir=vec4(teta*cos(p),teta*sin(p),cos(t),0.0);

	y=0.0;
	x=0.0;
	way=0.0;

	for (float ti=start;(ti<=time);ti+=step){

	
			way=pway*(speed*ti)/pway;
			x=offset*(speed*ti)/pway;
			y=radius*sin(x*2.0*3.14/periode);
		

		while(way<=(speed*ti)){
			
			x+=step2;//ElectronPosition.x
			y=radius*sin(x*2.0*3.14/periode);//ElectronPosition.y
			way+=sqrt(pow(step2,2.0)+pow(y-radius*sin((x+step2)*2.0*3.14/periode),2.0));
			
		}
		
		alpha=radius*cos(2.0*3.14/periode*x)*2.0*3.14/periode;
		delta=(speed*step*normalize(vec4(-1.0,alpha,0.0,0.0)));
		//delta=vec4(0.0,0.0,0.0,0.0);
		
	
		gl_FrontColor  = vec4((time-ti),(time-ti),(time-ti),1.0);
		
		pos=(vec4(x,y,0.0,1.0)
				+(normalize(dir*step+
					delta)
			)
			*(ti-start));
		
		
		gl_Position=gl_ModelViewProjectionMatrix*scalemat*(pos-vec4(offset*(speed*time)/pway,0.0,0.0,0.0));


		Normal=cross(lastpos.xyz-vec4(x,y,0.0,1.0).xyz,cross(vec4(x,y,0.0,1.0).xyz-gl_LightSource[0].position.xyz,lastpos.xyz-vec4(x,y,0.0,1.0).xyz));
		lastpos=pos;
		

		EmitVertex();
	}
	EndPrimitive();
}
}

}
