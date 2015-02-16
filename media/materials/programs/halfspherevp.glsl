const mat4 scalemat=mat4(	0.05, 0.0, 0.0, 0.0,
				0.0, 0.05, 0.0, 0.0,
				0.0, 0.0, 0.05, 0.0,
				0.0, 0.0, 0.0, 1.0);
varying vec3 Normal;

   void main( void )
   {

    gl_Position    = gl_ModelViewProjectionMatrix *scalemat* gl_Vertex;
	gl_FrontColor  = gl_Color;
      Normal         = gl_Normal;
   }