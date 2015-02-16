// simple fragment shader

// 'time' contains seconds since the program was linked.
uniform float width;
varying float coord;
void main()
{
	float r,b,a;
	r=0.0;
	b=0.0;
	a=0.0;
	//float g;
	//g=0.15;
	if (coord>=0.0 && coord<width){

	b=0.0;
	r=coord/width;
	a=1.0;
	}
	if(coord<0.0 && coord>-width){

	r=0.0;
	b=-coord/width;
	a=1.0;
	}
	if(coord<=-width && coord>=(-2.0*width)){
	b=2.0*(1.0+(coord/(2.0*width)));
	r=0.0;
	a=1.0;
	}
	if(coord>=width && coord<=(2.0*width)){
	r=2.0*(1.0-(coord/(2.0*width)));
	b=0.0;
	a=1.0;
	}
	
	gl_FragColor = vec4(r,0.0,b,a);
}
