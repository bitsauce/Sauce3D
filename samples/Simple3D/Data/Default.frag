in vec2 v_TexCoord;
in vec4 v_VertexColor;
			
out vec4 out_FragColor;

struct DirectionalLight
{
	vec3 direction;
	vec3 color;
};
			
uniform sampler2D u_Texture;
uniform DirectionalLight u_DirLight;

void main()
{
	//out_FragColor = texture(u_Texture, v_TexCoord) * v_VertexColor;
	out_FragColor = texture(u_Texture, v_TexCoord) * vec4(u_DirLight.color, 1.0);
}