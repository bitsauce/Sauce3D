in vec3 in_Position;
in vec2 in_TexCoord;
in vec3 in_Normal;
			
out vec2 v_TexCoord;
out vec3 v_FragWorldPos;
out vec3 v_Normal;
			
uniform mat4 u_ModelViewProj;
uniform mat4 u_ModelMatrix;
			
void main()
{
	gl_Position = vec4(in_Position, 1.0) * u_ModelViewProj;
	v_FragWorldPos = vec3(vec4(in_Position, 1.0) * u_ModelMatrix);
	v_TexCoord = in_TexCoord;
	v_Normal = in_Normal;
}
