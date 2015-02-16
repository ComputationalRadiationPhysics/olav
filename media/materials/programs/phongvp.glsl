 uniform vec3 LightPosition;
   uniform vec3 EyePosition;

   varying vec3 ViewDirection;
   varying vec3 LightDirection;
   varying vec3 Normal;
	varying vec3 color;

   void main( void )
   {

      

      vec4 ObjectPosition = gl_ModelViewMatrix * gl_Vertex;

      ViewDirection  = EyePosition - ObjectPosition.xyz;
      LightDirection = LightPosition - ObjectPosition.xyz;
//      Normal         = gl_NormalMatrix * gl_Normal;
      Normal         = gl_Normal;
	color=gl_Color;
      gl_Position = ftransform();
   }