#version 330 compatibility
// simple vertex shader
uniform float maxlength;

out VertexAttrib
{
  vec3 normal;
  vec4 color;
  
} vertex;
void main()
{
	//gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position    = gl_Vertex;
	gl_FrontColor  = gl_Color;
	//gl_Normal = gl_Normal;//neu
	vertex.color=gl_Color;
	vertex.normal=gl_Normal;
	

}
