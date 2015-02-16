// simple vertex shader
const mat4 scalemat=mat4(	0.05, 0.0, 0.0, 0.0,
				0.0, 0.05, 0.0, 0.0,
				0.0, 0.0, 0.05, 0.0,
				0.0, 0.0, 0.0, 1.0);

void main()
{


	gl_Position    = gl_ModelViewProjectionMatrix *scalemat* gl_Vertex;
	gl_FrontColor  = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
