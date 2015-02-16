varying vec3 Normal; 
void main( void )
   {
//if (normalize(Normal).z<-0.3) gl_FragColor=vec4(0.0,0.0,0.0,0.0);
  //    else gl_FragColor= gl_Color;
if (normalize(Normal).z>0.0)
gl_FragColor=vec4(gl_Color.x*(1.0-abs(normalize(Normal).z)),gl_Color.y*(1.0-abs(normalize(Normal).z)),gl_Color.z*(1.0-abs(normalize(Normal).z)),(1.0-abs(normalize(Normal).z)));
else
gl_FragColor=vec4(0.0,0.0,0.0,0.0);
//gl_FragColor=vec4(gl_Color.x*(abs(normalize(Normal).z)),gl_Color.y*(abs(normalize(Normal).z)),gl_Color.z*(abs(normalize(Normal).z)),0.0);
   }