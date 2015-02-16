// simple fragment shader

// 'time' contains seconds since the program was linked.
uniform float time;
   uniform vec4 amb;
   uniform vec4 spec;
   uniform vec4 diff;
   uniform vec4 color;
   in vec3 Normal; //N
   uniform vec4 ViewDirection; //V
   uniform vec4 LightDirection;
   uniform int blinnphong;
uniform float shin;

   void main( void )
   {

      vec3  LightDir = normalize( LightDirection.xyz );
      vec3  Normal         = normalize( Normal );
      float NDotL          = dot( Normal, LightDirection.xyz );

      vec3  Reflection     = normalize( ( ( 2.0 * Normal ) * NDotL ) - LightDir.xyz ); //R 
      
      vec3  ViewDirection  = normalize( ViewDirection.xyz);
      float RDotV          = max( 0.0, dot( Reflection, ViewDirection.xyz ) );

      vec4  TotalAmbient   = amb * color*gl_Color;
      vec4  TotalDiffuse   = diff * NDotL * color*gl_Color;
      vec4 TotalSpecular;    
       
      vec3 Halfvector      = (ViewDirection.xyz + LightDir.xyz)/normalize(ViewDirection.xyz + LightDir.xyz); //H

      float NDotH          = max( 0.0, dot( Normal, normalize(Halfvector) ) );
      
      if (blinnphong == 1){
      TotalSpecular = spec * color*gl_Color * ( pow( NDotH, shin ) ) ;
      }
      else{
      TotalSpecular  = spec * color*gl_Color * ( pow( RDotV, shin ) ) ;
       }
      

      gl_FragColor = ( TotalAmbient + TotalDiffuse + TotalSpecular );
//gl_FragColor=gl_Color;
   }
