#version 420

in vec2 v_TexCoord;
in vec4 v_VertexColor;

out vec4 out_FragColor;

uniform sampler2D u_Texture;
//uniform float u_EdgeSharpness;
//
//void main()
//{
//	float sdfValue = texture(u_Texture, v_TexCoord*2).r;
//	float alpha = smoothstep(u_EdgeSharpness * 0.5f, 1.0f - u_EdgeSharpness * 0.5f, sdfValue);
//	out_FragColor = vec4(v_VertexColor.rgb, alpha);
//}
//

uniform float u_Edge0;
uniform float u_Edge1;
uniform float u_Scale;

void main()
{
	float sdfValue = texture(u_Texture, v_TexCoord*u_Scale).r;
	float alpha = smoothstep(u_Edge0, u_Edge1, sdfValue);
	out_FragColor = vec4(v_VertexColor.rgb, alpha);
}
