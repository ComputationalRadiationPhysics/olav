uniform vec4 amb;
uniform vec4 spec;
uniform vec4 diff;
  // uniform vec4 color;
uniform int blinnphong;
uniform float shin;

   varying vec3 ViewDirection; //V
   varying vec3 LightDirection; //L
	varying vec3 color;
   varying vec3 Normal; //N

   void main( void )
   {

      vec3  LightDirection = normalize( LightDirection );
      vec3  Normal         = normalize( Normal );
      float NDotL          = dot( Normal, LightDirection );

      vec3  Reflection     = normalize( ( ( 2.0 * Normal ) * NDotL ) - LightDirection ); //R 
      
      vec3  ViewDirection  = normalize( ViewDirection);
      float RDotV          = max( 0.0, dot( Reflection, ViewDirection ) );

      vec4  TotalAmbient   = amb * color;
      vec4  TotalDiffuse   = diff * NDotL * color;
      vec4 TotalSpecular;    
       
      vec3 Halfvector      = (ViewDirection + LightDirection)/normalize(ViewDirection + LightDirection); //H

      float NDotH          = max( 0.0, dot( Normal, normalize(Halfvector) ) );
      
      if (blinnphong == 1){
      TotalSpecular = spec * color * ( pow( NDotH, shin ) ) ;
      }
      else{
      TotalSpecular  = spec * color * ( pow( RDotV, shin ) ) ;
       }
      

      gl_FragColor = ( TotalAmbient + TotalDiffuse + TotalSpecular );

   }
