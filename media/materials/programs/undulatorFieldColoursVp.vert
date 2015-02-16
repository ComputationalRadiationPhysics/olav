// simple vertex shader

uniform mat4 matrix;
uniform vec4 clipnormal;
uniform vec4 offset;
varying float coord;
void main()
{
//	float r,b;
	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;

	vec4 v=matrix*gl_Vertex;
	float hnfD=(dot(clipnormal,v)-dot(clipnormal,offset))/length(clipnormal);
	coord=hnfD;
	
	/*r=1.0;
	b=1.0;
	if (coord>=0.0 && coord<width){
	//r=gl_Vertex.z/10.0;
	r=0.0;
	b=gl_Vertex.z/width;
	}
	if(coord<0.0 && coord>-width){
	//b=-gl_Vertex.z/10.0;
	b=0.0;
	r=-coord/width;
	}
	if(coord<-width && coord>(-2.0*width)){
	r=-1.0/(coord/(2.0*width));
	b=0.0;
	}
	if(coord>width && coord<(2.0*width)){
	b=1.0/(coord/(2.0*width));
	r=0.0;
	}*/
	//gl_FrontColor  = vec4(r,0.0,b,1.0);
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
